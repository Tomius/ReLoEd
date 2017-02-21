// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "./cdlod_quad_tree_node.h"
#include "../camera.h"
#include "../settings.h"

namespace engine {

class CdlodQuadTree {
  size_t max_node_level_;
  CdlodQuadTreeNode root_;

 public:
  CdlodQuadTree(size_t kFaceSize, CubeFace face);
  CdlodQuadTree(CdlodQuadTree&&) = default;

  void render(const engine::Camera& cam, QuadGridMesh& mesh,
              ThreadPool& thread_pool);
  size_t max_node_level() const { return max_node_level_; }
};

}  // namespace engine

#endif
