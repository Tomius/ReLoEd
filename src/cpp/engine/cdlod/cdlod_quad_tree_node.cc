// Copyright (c) 2015, Tamas Csala

#include <algorithm>
#include "./cdlod_quad_tree_node.h"
#include "../misc.h"
#include "../collision/cube2sphere.h"

#define gl(func) OGLWRAP_CHECKED_FUNCTION(func)

namespace engine {

CdlodQuadTreeNode::CdlodQuadTreeNode(double x, double z,
                                     CubeFace face, int level)
    : x_(x), z_(z), face_(face), level_(level) {
  bbox_ = SpherizedAABB{BoundingBox{
    {x-size()/2, 0, z-size()/2},
    {x+size()/2, Settings::max_height, z+size()/2}
  }, face, Settings::face_size};
}

void CdlodQuadTreeNode::initChild(int i) {
  assert (0 <= i && i <= 3);

  double s4 = size()/4;
  if (i == 0) {
    children_[0] = make_unique<CdlodQuadTreeNode>(x_-s4, z_+s4, face_, level_-1);
  } else if (i == 1) {
    children_[1] = make_unique<CdlodQuadTreeNode>(x_+s4, z_+s4, face_, level_-1);
  } else if (i == 2) {
    children_[2] = make_unique<CdlodQuadTreeNode>(x_-s4, z_-s4, face_, level_-1);
  } else if (i == 3) {
    children_[3] = make_unique<CdlodQuadTreeNode>(x_+s4, z_-s4, face_, level_-1);
  }
}

void CdlodQuadTreeNode::selectNodes(const glm::vec3& cam_pos,
                                    const Frustum& frustum,
                                    QuadGridMesh& grid_mesh,
                                    uint64_t texture_id /*= 0*/,
                                    glm::vec3 texture_info /*= glm::vec3{}*/) {
  if (!bbox_.collidesWithFrustum(frustum)) { return; }

  last_used_ = 0;
  float lod_range = Settings::lod_level_distance_multiplier * size();
  selectTexture(cam_pos, frustum, texture_id, texture_info);

  // If we can cover the whole area or if we are a leaf
  Sphere sphere{cam_pos, lod_range};
  if (!bbox_.collidesWithSphere(sphere) || level_ <= -Settings::geom_div) {
    grid_mesh.addToRenderList(x_, z_, scale(), level_, texture_id, texture_info);
  } else {
    bool cc[4]{}; // children collision

    for (int i = 0; i < 4; ++i) {
      if (!children_[i])
        initChild(i);

      cc[i] = children_[i]->collidesWithSphere(sphere);
      if (cc[i]) {
        // Ask child to render what we can't
        children_[i]->selectNodes(cam_pos, frustum, grid_mesh,
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
                                      uint64_t& texture_id,
                                      glm::vec3& texture_info) {
  if (textureLevel() >= 1) {
    if (!is_texture_uploaded_) {
      gl::Bind(texture_);
      Magick::Image image{getHeightMapPath()};
      std::vector<unsigned short> data;
      size_t w = image.columns(), h = image.rows();
      data.resize(w*h);
      image.write(0, 0, w, h, "R", MagickCore::ShortPixel, data.data());
      texture_.upload(gl::kR16, w, h, gl::kRed, gl::kUnsignedShort, data.data());

      texture_.generateMipmap();
      texture_.maxAnisotropy();
      texture_.minFilter(gl::kLinearMipmapLinear);
      texture_.magFilter(gl::kLinear);
      texture_.wrapS(gl::kClampToEdge);
      texture_.wrapT(gl::kClampToEdge);

      texture_id_ = gl(GetTextureHandleARB(texture_.expose()));
      gl(MakeTextureHandleResidentARB(texture_id_));
      gl::Unbind(texture_);

      double scale = (Settings::kTextureDimension+2.0*Settings::kTextureBorderSize) / Settings::kTextureDimension;
      double s2 = scale*size()/2;
      texture_info_ = glm::vec3(x_ - s2, z_ - s2, 2*s2);

      is_texture_uploaded_ = true;
    }

    Settings::texture_nodes_count++;
    texture_id = texture_id_;
    texture_info = texture_info_;
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
  return std::max(level_ - (Settings::kTextureDimensionExp - Settings::node_dimension_exp), 0);
}

}  // namespace engine

#undef gl
