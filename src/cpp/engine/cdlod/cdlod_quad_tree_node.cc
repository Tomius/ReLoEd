// Copyright (c) 2015, Tamas Csala

#include <algorithm>
#include "./cdlod_quad_tree_node.h"
#include "../misc.h"
#include "../collision/cube2sphere.h"

namespace engine {

CdlodQuadTreeNode::CdlodQuadTreeNode(double x, double z,
                                     CubeFace face, int level)
    : x_(x), z_(z), face_(face), level_(level) {
  bbox_ = SpherizedAABB{BoundingBox{
    {x-size()/2, 0, z-size()/2},
    {x+size()/2, GlobalHeightMap::max_height, z+size()/2}
  }, face, GlobalHeightMap::face_size};
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
                                    QuadGridMesh& grid_mesh) {
  if (!bbox_.collidesWithFrustum(frustum)) { return; }

  last_used_ = 0;
  float lod_range = GlobalHeightMap::lod_level_distance_multiplier * size();

  // If we can cover the whole area or if we are a leaf
  Sphere sphere{cam_pos, lod_range};
  if (!bbox_.collidesWithSphere(sphere) || level_ <= -GlobalHeightMap::geom_div) {
    grid_mesh.addToRenderList(x_, z_, scale(), level_);
  } else {
    bool cc[4]{}; // children collision

    for (int i = 0; i < 4; ++i) {
      if (!children_[i])
        initChild(i);

      cc[i] = children_[i]->collidesWithSphere(sphere);
      if (cc[i]) {
        // Ask child to render what we can't
        children_[i]->selectNodes(cam_pos, frustum, grid_mesh);
      }
    }

    // Render what the children didn't do
    grid_mesh.addToRenderList(x_, z_, scale(), level_,
                              !cc[0], !cc[1], !cc[2], !cc[3]);
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

}  // namespace engine
