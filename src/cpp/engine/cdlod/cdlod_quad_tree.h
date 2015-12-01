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
  CdlodQuadTreeNode root_;
  GLubyte max_node_level(size_t w, size_t h) const;

 public:
  CdlodQuadTree(size_t w, size_t h, CubeFace face);
  void render(const engine::Camera& cam, QuadGridMesh& mesh);
};

}  // namespace engine

#endif
