// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_QUAD_TREE_NODE_H_

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "../global_height_map.h"
#include "../collision/spherized_aabb.h"

namespace engine {

class CdlodQuadTreeNode {
 public:
  CdlodQuadTreeNode(double x, double z, CubeFace face, int level);

  double scale() const { return pow(2, level_); }
  double size() { return GlobalHeightMap::node_dimension * scale(); }

  bool collidesWithSphere(const Sphere& sphere) const {
    return bbox_.collidesWithSphere(sphere);
  }

  void age();
  void selectNodes(const glm::vec3& cam_pos,
                   const Frustum& frustum,
                   QuadGridMesh& grid_mesh);

 private:
  using BBox = SpherizedAABBSat<GlobalHeightMap::tex_w, GlobalHeightMap::tex_h>;

  double x_, z_;
  CubeFace face_;
  int level_;

  BBox bbox_;
  std::unique_ptr<CdlodQuadTreeNode> children_[4];
  int last_used_ = 0;

  // If a node is not used for this much time (frames), it will be unloaded.
  static const int kTimeToLiveInMemory = 1 << 8;

  void initChild(int i);
};

}

#endif
