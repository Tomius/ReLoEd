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
  bbox_ = SpherizedAABB{
    {x-size()/2, 0, z-size()/2},
    {x+size()/2, Settings::kMaxHeight, z+size()/2},
    face, Settings::kFaceSize};
}

CdlodQuadTreeNode::~CdlodQuadTreeNode() {
  if (texture_.is_loaded_to_gpu) {
    Settings::texture_nodes_count--;
    if (texture_.elevation.size != 0) {
      gl(MakeTextureHandleNonResidentARB(texture_.elevation.id));
    }
    if (texture_.diffuse.size != 0) {
      gl(MakeTextureHandleNonResidentARB(texture_.diffuse.id));
    }
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
  last_used_ = 0;

  StreamedTextureInfo texinfo;
  selectTexture(cam_pos, frustum, thread_pool, texinfo);

  // textures should be loaded, even if it is outside the frustum (otherwise the
  // texture lod difference of neighbour nodes can cause geometry cracks)
  // if (!bbox_.collidesWithFrustum(frustum)) { return; }

  // If we can cover the whole area or if we are a leaf
  Sphere sphere{cam_pos, Settings::kSmallestGeometryLodDistance * scale()};
  if (!bbox_.collidesWithSphere(sphere) ||
      level_ <= Settings::kLevelOffset - Settings::kGeomDiv) {
    if (bbox_.collidesWithFrustum(frustum)) {
      grid_mesh.addToRenderList(x_, z_, level_, int(face_), texinfo);
    }
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

    if (bbox_.collidesWithFrustum(frustum)) {
      // Render what the children didn't do
      grid_mesh.addToRenderList(x_, z_, level_, int(face_), texinfo,
                                !cc[0], !cc[1], !cc[2], !cc[3]);
    }
  }
}


void CdlodQuadTreeNode::selectTexture(const glm::vec3& cam_pos,
                                      const Frustum& frustum,
                                      ThreadPool& thread_pool,
                                      StreamedTextureInfo& texinfo,
                                      int recursion_level /*= 0*/) {
  bool need_geometry = (texinfo.geometry_current == nullptr);
  bool need_normal   = (texinfo.normal_current == nullptr);
  bool need_diffuse  = (texinfo.diffuse_current == nullptr);

  // nothing more to find
  if (!need_geometry && !need_normal && !need_diffuse) {
    return;
  }

  if (parent_ == nullptr) {
    if (!texture_.is_loaded_to_gpu) {
      loadTexture(true);
      upload();
    }

    if (need_geometry) {
      texinfo.geometry_current = &texture_.elevation;
      texinfo.geometry_next = &texture_.elevation;
    }
    if (need_normal) {
      texinfo.normal_current = &texture_.elevation;
      texinfo.normal_next = &texture_.elevation;
    }
    if (need_diffuse) {
      texinfo.diffuse_current = &texture_.diffuse;
      texinfo.diffuse_next = &texture_.diffuse;
    }

    return;
  }

  bool too_detailed = recursion_level < Settings::kTexDimOffset - Settings::kNormalToGeometryLevelOffset;
  bool too_detailed_for_geometry = recursion_level < Settings::kTexDimOffset;

  bool can_use_geometry = need_geometry && hasElevationTexture() && !too_detailed_for_geometry;
  bool can_use_normal = need_normal && hasElevationTexture() && !too_detailed;
  bool can_use_diffuse = need_diffuse && hasDiffuseTexture() && !too_detailed;

  if (can_use_geometry || can_use_normal || can_use_diffuse) {
    if (!texture_.is_loaded_to_gpu && texture_.is_loaded_to_memory) {
      upload();
    }

    if (texture_.is_loaded_to_gpu) {
      assert(parent_->texture_.is_loaded_to_gpu);

      if (can_use_geometry) {
        texinfo.geometry_current = &texture_.elevation;
        texinfo.geometry_next = &parent_->texture_.elevation;
      }

      if (can_use_normal) {
        texinfo.normal_current = &texture_.elevation;
        texinfo.normal_next = &parent_->texture_.elevation;
      }

      if (can_use_diffuse) {
        texinfo.diffuse_current = &texture_.diffuse;
        texinfo.diffuse_next = &parent_->texture_.diffuse;
      }
    } else {
      // this one should be used, but not yet loaded -> start async load, but
      // make do with the parent for now.
      thread_pool.enqueue(level_, [this](){ loadTexture(false); });
    }
  }

  parent_->selectTexture(cam_pos, frustum, thread_pool,
                         texinfo, recursion_level+1);
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

std::string CdlodQuadTreeNode::getDiffuseMapPath() const {
  assert(hasDiffuseTexture());
  return std::string{"/home/icecool/projects/C++/OpenGL/ReLoEd/src/resources/textures/diffuse"}
         + "/" + std::to_string(int(face_))
         + "/" + std::to_string(diffuseTextureLevel())
         + "/" + std::to_string(long(x_) >> Settings::kDiffuseToElevationLevelOffset)
         + "/" + std::to_string(long(z_) >> Settings::kDiffuseToElevationLevelOffset)
         + ".png";
}


std::string CdlodQuadTreeNode::getHeightMapPath() const {
  assert(hasElevationTexture());
  return std::string{"/media/icecool/SSData/gmted2010_75_cube"}
         + "/" + std::to_string(int(face_))
         + "/" + std::to_string(elevationTextureLevel())
         + "/" + std::to_string(long(x_))
         + "/" + std::to_string(long(z_))
         + ".png";
}

int CdlodQuadTreeNode::elevationTextureLevel() const {
  return level_ - Settings::kTexDimOffset;
}

int CdlodQuadTreeNode::diffuseTextureLevel() const {
  return elevationTextureLevel() - Settings::kDiffuseToElevationLevelOffset;
}

bool CdlodQuadTreeNode::hasElevationTexture() const {
  return Settings::kLevelOffset <= elevationTextureLevel();
}

bool CdlodQuadTreeNode::hasDiffuseTexture() const {
  return Settings::kLevelOffset <= diffuseTextureLevel();
}

void CdlodQuadTreeNode::loadTexture(bool synchronous_load) {
  if (parent_ && !parent_->texture_.is_loaded_to_memory) {
    parent_->loadTexture(synchronous_load);
  }
  if (texture_.is_loaded_to_gpu) {
    return;
  }

  if (synchronous_load) {
    texture_.load_mutex.lock();
  } else {
    if (!texture_.load_mutex.try_lock()) {
      // someone is already loading this texture, so we should not wait on it
      return;
    }
  }

  if (!texture_.is_loaded_to_memory) {
    if (hasElevationTexture()) {
      Magick::Image image{getHeightMapPath()};
      assert(image.columns() == Settings::kElevationTexSizeWithBorders);
      assert(image.rows() == Settings::kElevationTexSizeWithBorders);
      texture_.elevation_data.resize(image.columns() * image.rows());
      image.write(0, 0, image.columns(), image.rows(),
                  "R", MagickCore::ShortPixel, texture_.elevation_data.data());
    }

    if (hasDiffuseTexture()) {
      Magick::Image image{getDiffuseMapPath()};
      assert(image.columns() == Settings::kDiffuseTexSizeWithBorders);
      assert(image.rows() == Settings::kDiffuseTexSizeWithBorders);
      texture_.diffuse_data.resize(image.columns() * image.rows());
      image.write(0, 0, image.columns(), image.rows(),
                  "RGB", MagickCore::CharPixel, texture_.diffuse_data.data());
    }

    texture_.is_loaded_to_memory = true;
  }

  texture_.load_mutex.unlock();
}

void CdlodQuadTreeNode::upload() {
  loadTexture(true);
  if (parent_ && !parent_->texture_.is_loaded_to_gpu) {
    parent_->upload();
  }

  if (!texture_.is_loaded_to_gpu) {
    if (hasElevationTexture()) {
      gl::Bind(texture_.elevation.handle);
      texture_.elevation.handle.upload(
          gl::kR16, Settings::kElevationTexSizeWithBorders,
          Settings::kElevationTexSizeWithBorders, gl::kRed,
          gl::kUnsignedShort, texture_.elevation_data.data());

      texture_.elevation.handle.generateMipmap();
      texture_.elevation.handle.maxAnisotropy();
      texture_.elevation.handle.minFilter(gl::kLinearMipmapLinear);
      texture_.elevation.handle.magFilter(gl::kLinear);
      texture_.elevation.handle.wrapS(gl::kClampToEdge);
      texture_.elevation.handle.wrapT(gl::kClampToEdge);

      texture_.elevation.id =
          gl(GetTextureHandleARB(texture_.elevation.handle.expose()));
      gl(MakeTextureHandleResidentARB(texture_.elevation.id));
      gl::Unbind(texture_.elevation.handle);

      double scale = static_cast<double>(Settings::kElevationTexSizeWithBorders)
                   / static_cast<double>(Settings::kTextureDimension);
      texture_.elevation.size = scale * size();
      texture_.elevation.position = glm::vec2(x_ - texture_.elevation.size/2,
                                              z_ - texture_.elevation.size/2);

      texture_.elevation_data.clear();
    }

    if (hasDiffuseTexture()) {
      gl::Bind(texture_.diffuse.handle);
      texture_.diffuse.handle.upload(
          gl::kRgb8, Settings::kDiffuseTexSizeWithBorders,
          Settings::kDiffuseTexSizeWithBorders, gl::kRgb,
          gl::kUnsignedByte, texture_.diffuse_data.data());

      texture_.diffuse.handle.generateMipmap();
      texture_.diffuse.handle.maxAnisotropy();
      texture_.diffuse.handle.minFilter(gl::kLinearMipmapLinear);
      texture_.diffuse.handle.magFilter(gl::kLinear);
      texture_.diffuse.handle.wrapS(gl::kClampToEdge);
      texture_.diffuse.handle.wrapT(gl::kClampToEdge);

      texture_.diffuse.id =
          gl(GetTextureHandleARB(texture_.diffuse.handle.expose()));
      gl(MakeTextureHandleResidentARB(texture_.diffuse.id));
      gl::Unbind(texture_.diffuse.handle);

      double scale = static_cast<double>(Settings::kDiffuseTexSizeWithBorders)
                   / static_cast<double>(Settings::kTextureDimension);
      texture_.diffuse.size = scale * size();
      texture_.diffuse.position = glm::vec2(x_ - texture_.diffuse.size/2,
                                            z_ - texture_.diffuse.size/2);

      texture_.diffuse_data.clear();
    }

    texture_.is_loaded_to_gpu = true;
    Settings::texture_nodes_count++;
  }
}


}  // namespace engine

#undef gl
