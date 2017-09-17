// Copyright (c), Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_H_
#define ENGINE_CDLOD_QUAD_TREE_H_

#include <memory>
#include <Silice3D/camera/icamera.hpp>

#include "cdlod/geometry/quad_grid_mesh.hpp"
#include "cdlod/cdlod_quad_tree_node.hpp"

namespace Cdlod {

class CdlodQuadTree {
  size_t max_node_level_;
  CdlodQuadTreeNode root_;

 public:
  CdlodQuadTree(size_t kFaceSize, CubeFace face);
  CdlodQuadTree(CdlodQuadTree&&) = default;

  void render(const Silice3D::ICamera& cam, QuadGridMesh& mesh,
              Silice3D::ThreadPool& thread_pool);
  size_t max_node_level() const { return max_node_level_; }
};

} // namespace Cdlod

#endif
