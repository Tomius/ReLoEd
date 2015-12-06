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

  children_[i] = make_unique<CdlodQuadTreeNode>(x, z, face_, level_-1, parent_);
}

void CdlodQuadTreeNode::selectNodes(const glm::vec3& cam_pos,
                                    const Frustum& frustum,
                                    QuadGridMesh& grid_mesh,
                                    ThreadPool& thread_pool,
                                    uint64_t texture_id,
                                    glm::vec3 texture_info) {
  if (!bbox_.collidesWithFrustum(frustum)) { return; }

  last_used_ = 0;
  float lod_range = Settings::kSmallestGeometryLodDistance * scale();

  // uint64_t texture_id;
  // glm::vec3 texture_info;
  selectTexture(cam_pos, frustum, thread_pool, texture_id, texture_info);

  // If we can cover the whole area or if we are a leaf
  Sphere sphere{cam_pos, lod_range};
  if (!bbox_.collidesWithSphere(sphere) || level_ <= -Settings::kGeomDiv) {
    grid_mesh.addToRenderList(x_, z_, scale(), level_, texture_id, texture_info);
  } else {
    bool cc[4]{}; // children collision

    for (int i = 0; i < 4; ++i) {
      if (!children_[i])
        initChild(i);

      cc[i] = children_[i]->collidesWithSphere(sphere);
      if (cc[i]) {
        // Ask child to render what we can't
        children_[i]->selectNodes(cam_pos, frustum, grid_mesh, thread_pool,
                                  texture_id, texture_info);
      }
    }

    // Render what the children didn't do
    grid_mesh.addToRenderList(x_, z_, scale(), level_, texture_id, texture_info,
                              !cc[0], !cc[1], !cc[2], !cc[3]);
  }
}


void CdlodQuadTreeNode::selectTexture(const glm::vec3& cam_pos,
                                      const Frustum& frustum,
                                      ThreadPool& thread_pool,
                                      uint64_t& texture_id,
                                      glm::vec3& texture_info) {
  if (textureLevel() == 8) {
    loadTexture();
    upload();
  }

  if (textureLevel() >= 1) {
    if (is_loaded_to_gpu_) {
      Settings::texture_nodes_count++;
      texture_id = texture_id_;
      texture_info = texture_info_;
    } else if (is_loaded_to_memory_) {
      upload();
    } else {
      thread_pool.enqueue([this](){
        loadTexture();
      });
    }
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
  return std::max(level_ - (Settings::kTextureDimensionExp - Settings::kNodeDimensionExp), 0);
}

void CdlodQuadTreeNode::loadTexture() {
  std::unique_lock<std::mutex> lock{load_mutex_};

  if (!is_loaded_to_memory_) {
    Magick::Image image{getHeightMapPath()};
    tex_w = image.columns(), tex_h = image.rows();
    data_.resize(tex_w * tex_h);
    image.write(0, 0, tex_w, tex_h, "R", MagickCore::ShortPixel, data_.data());

    is_loaded_to_memory_ = true;
  }
}

void CdlodQuadTreeNode::upload() {
  if (!is_loaded_to_gpu_) {
    gl::Bind(texture_);
    texture_.upload(gl::kR16, tex_w, tex_h, gl::kRed,
                    gl::kUnsignedShort, data_.data());

    texture_.generateMipmap();
    texture_.maxAnisotropy();
    texture_.minFilter(gl::kLinearMipmapLinear);
    texture_.magFilter(gl::kLinear);
    texture_.wrapS(gl::kClampToEdge);
    texture_.wrapT(gl::kClampToEdge);

    texture_id_ = gl(GetTextureHandleARB(texture_.expose()));
    gl(MakeTextureHandleResidentARB(texture_id_));
    gl::Unbind(texture_);

    double scale = (Settings::kTextureDimension+2.0*Settings::kTextureBorderSize)
                   / Settings::kTextureDimension;
    double scaled_size = scale * size();
    texture_info_ = glm::vec3(x_ - scaled_size/2, z_ - scaled_size/2, scaled_size);

    is_loaded_to_gpu_ = true;
  }
}


}  // namespace engine

#undef gl
