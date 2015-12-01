// Copyright (c) 2015, Tamas Csala

#include "./cdlod_quad_tree.h"

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "./cdlod_quad_tree_node.h"
#include "../camera.h"
#include "../global_height_map.h"

namespace engine {

GLubyte CdlodQuadTree::max_node_level(size_t w, size_t h) const {
  int x_depth = 0;
  while (size_t(GlobalHeightMap::node_dimension << x_depth) < w) {
    x_depth++;
  }
  int y_depth = 0;
  while (size_t(GlobalHeightMap::node_dimension << y_depth) < h) {
    y_depth++;
  }

  return std::max(x_depth, y_depth);
}

CdlodQuadTree::CdlodQuadTree(size_t w, size_t h, CubeFace face)
  : root_(w/2, h/2, face, max_node_level(w, h)) {}

void CdlodQuadTree::render(const engine::Camera& cam, QuadGridMesh& mesh) {
  root_.selectNodes(cam.transform()->pos(), cam.frustum(), mesh);
  root_.age();
}

}  // namespace engine

