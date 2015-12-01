// Copyright (c) 2015, Tamas Csala

#include "./cdlod_quad_tree.h"

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "./cdlod_quad_tree_node.h"
#include "../camera.h"
#include "../global_height_map.h"

namespace engine {

CdlodQuadTree::CdlodQuadTree(size_t face_size, CubeFace face)
  : max_node_level_(log2(face_size) - GlobalHeightMap::node_dimension_exp)
  , root_(face_size/2, face_size/2, face, max_node_level_) {}

void CdlodQuadTree::render(const engine::Camera& cam, QuadGridMesh& mesh) {
  root_.selectNodes(cam.transform()->pos(), cam.frustum(), mesh);
  root_.age();
}

}  // namespace engine

