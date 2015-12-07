// Copyright (c) 2015, Tamas Csala

#include <algorithm>
#include "./cdlod_quad_tree_node.h"
#include "../misc.h"
#include "../collision/cube2sphere.h"

#define gl(func) OGLWRAP_CHECKED_FUNCTION(func)

namespace engine {

CdlodQuadTreeNode::CdlodQuadTreeNode(double x, double z, CubeFace face,
                                     int level, CdlodQuadTreeNode* parent)
    : x_(x), z_(z), face_(face), level_(level), parent_(parent) {
  bbox_ = SpherizedAABB{BoundingBox{
    {x-size()/2, 0, z-size()/2},
    {x+size()/2, Settings::kMaxHeight, z+size()/2}
  }, face, Settings::kFaceSize};
}

CdlodQuadTreeNode::~CdlodQuadTreeNode() {
  if (texture_.is_loaded_to_gpu) {
    Settings::texture_nodes_count--;
  }
}

void CdlodQuadTreeNode::initChild(int i) {
  assert (0 <= i && i <= 3);

  double s4 = size()/4, x, z;
  if (i == 0) {
    x = x_-s4; z = z_+s4;
  } else if (i == 1) {
    x = x_+s4; z = z_+s4;
  } else if (i == 2) {
    x = x_-s4; z = z_-s4;
  } else if (i == 3) {
    x = x_+s4; z = z_-s4;
  }

  children_[i] = make_unique<CdlodQuadTreeNode>(x, z, face_, level_-1, this);
}

void CdlodQuadTreeNode::selectNodes(const glm::vec3& cam_pos,
                                    const Frustum& frustum,
                                    QuadGridMesh& grid_mesh,
                                    ThreadPool& thread_pool) {
  if (!bbox_.collidesWithFrustum(frustum)) { return; }

  last_used_ = 0;
  double lod_range = Settings::kSmallestGeometryLodDistance * scale();

  StreamedTextureInfo texinfo;
  selectTexture(cam_pos, frustum, thread_pool, texinfo);

  // If we can cover the whole area or if we are a leaf
  Sphere sphere{cam_pos, lod_range};
  if (!bbox_.collidesWithSphere(sphere) ||
      level_ <= Settings::kLevelOffset - Settings::kGeomDiv) {
    grid_mesh.addToRenderList(x_, z_, level_, int(face_), texinfo);
  } else {
    bool cc[4]{}; // children collision

    for (int i = 0; i < 4; ++i) {
      if (!children_[i])
        initChild(i);

      cc[i] = children_[i]->collidesWithSphere(sphere);
      if (cc[i]) {
        // Ask child to render what we can't
        children_[i]->selectNodes(cam_pos, frustum, grid_mesh, thread_pool);
      }
    }

    // Render what the children didn't do
    grid_mesh.addToRenderList(x_, z_, level_, int(face_), texinfo,
                              !cc[0], !cc[1], !cc[2], !cc[3]);
  }
}


void CdlodQuadTreeNode::selectTexture(const glm::vec3& cam_pos,
                                      const Frustum& frustum,
                                      ThreadPool& thread_pool,
                                      StreamedTextureInfo& texinfo,
                                      int recursion_level /*= 0*/) {
  if (parent_ == nullptr) {
    if (!texture_.is_loaded_to_gpu) {
      loadTexture();
      upload();
    }

    texinfo.geometry_current = &texture_;
    texinfo.geometry_next = &texture_;
    if (texinfo.normal_current == nullptr) {
      texinfo.normal_current = &texture_;
      texinfo.normal_next = &texture_;
    }
    return;
  }

  // if it is possible to get a texture from this
  // level, and it is not too detailed
  if (textureLevel() >= Settings::kLevelOffset &&
      recursion_level >= Settings::kTextureLodOffset) {
    if (!texture_.is_loaded_to_gpu && texture_.is_loaded_to_memory) {
      upload();
    }

    if (texture_.is_loaded_to_gpu) {
      assert(parent_->texture_.is_loaded_to_gpu);

      // we found one which exists and is loaded -> perfect
      if (texinfo.normal_current == nullptr) {
        texinfo.normal_current = &texture_;
        texinfo.normal_next = &parent_->texture_;
      }

      int tex_dim_offset = Settings::kTextureDimensionExp
                         - Settings::kNodeDimensionExp;
      if (recursion_level >= tex_dim_offset) {
        texinfo.geometry_current = &texture_;
        texinfo.geometry_next = &parent_->texture_;
      }
    } else {
      // this one should be used, but not yet loaded -> start async load, but
      // make do with the parent for now.
      thread_pool.enqueue([this](){
        loadTexture();
      });
    }
  }

  // if the geometry is filled we have everything we need,
  // else we need to continue the recursion
  if (texinfo.geometry_next == nullptr) {
    // the current one is not loaded or does not exist, so
    // the parent should provide the texture
    parent_->selectTexture(cam_pos, frustum, thread_pool,
                           texinfo, recursion_level+1);
  }
}

void CdlodQuadTreeNode::age() {
  last_used_++;

  for (auto& child : children_) {
    if (child) {
      // unload child if its age would exceed the ttl
      if (child->last_used_ >= kTimeToLiveInMemory) {
        child.reset();
      } else {
        child->age();
      }
    }
  }
}

std::string CdlodQuadTreeNode::getHeightMapPath() const {
  return std::string{"src/resources/height"}
         + "/" + std::to_string(int(face_))
         + "/" + std::to_string(textureLevel())
         + "/" + std::to_string(long(x_))
         + "/" + std::to_string(long(z_)) + ".png";
}

int CdlodQuadTreeNode::textureLevel() const {
  int offset = Settings::kTextureDimensionExp - Settings::kNodeDimensionExp;
  return std::max(level_ - offset, 0);
}

void CdlodQuadTreeNode::loadTexture() {
  if (parent_ && !parent_->texture_.is_loaded_to_memory) {
    parent_->loadTexture();
  }
  if (texture_.is_loaded_to_gpu) {
    return;
  }

  std::unique_lock<std::mutex> lock{texture_.load_mutex};

  if (!texture_.is_loaded_to_memory) {
    Magick::Image image{getHeightMapPath()};
    texture_.width = image.columns(), texture_.height = image.rows();
    texture_.data.resize(texture_.width * texture_.height);
    image.write(0, 0, texture_.width, texture_.height,
                "R", MagickCore::ShortPixel, texture_.data.data());

    texture_.is_loaded_to_memory = true;
  }
}

void CdlodQuadTreeNode::upload() {
  if (parent_ && !parent_->texture_.is_loaded_to_gpu) {
    parent_->upload();
  }

  assert(texture_.is_loaded_to_memory);
  if (!texture_.is_loaded_to_gpu) {
    gl::Bind(texture_.handle);
    texture_.handle.upload(gl::kR16, texture_.width, texture_.height,
                           gl::kRed, gl::kUnsignedShort, texture_.data.data());

    texture_.handle.generateMipmap();
    texture_.handle.maxAnisotropy();
    texture_.handle.minFilter(gl::kLinearMipmapLinear);
    texture_.handle.magFilter(gl::kLinear);
    texture_.handle.wrapS(gl::kClampToEdge);
    texture_.handle.wrapT(gl::kClampToEdge);

    texture_.id = gl(GetTextureHandleARB(texture_.handle.expose()));
    gl(MakeTextureHandleResidentARB(texture_.id));
    gl::Unbind(texture_.handle);

    double scale = (Settings::kTextureDimension+2.0*Settings::kTextureBorderSize)
                   / Settings::kTextureDimension;
    texture_.size = scale * size();
    texture_.position = glm::vec2(x_ - texture_.size/2, z_ - texture_.size/2);

    texture_.data.clear();
    texture_.is_loaded_to_gpu = true;
    Settings::texture_nodes_count++;
  }
}


}  // namespace engine

#undef gl
