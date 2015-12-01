// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "./cdlod_quad_tree_node.h"
#include "../camera.h"
#include "../global_height_map.h"

namespace engine {

class CdlodQuadTree {
  size_t max_node_level_;
  CdlodQuadTreeNode root_;

 public:
  CdlodQuadTree(size_t face_size, CubeFace face);
  void render(const engine::Camera& cam, QuadGridMesh& mesh);
  size_t max_node_level() const { return max_node_level_; }
};

}  // namespace engine

#endif
