// Copyright (c) 2015, Tamas Csala

#include "./cdlod_quad_tree.h"

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "./cdlod_quad_tree_node.h"
#include "../camera.h"
#include "../settings.h"

namespace engine {

CdlodQuadTree::CdlodQuadTree(size_t kFaceSize, CubeFace face)
  : max_node_level_(log2(kFaceSize) - Settings::kNodeDimensionExp)
  , root_(kFaceSize/2, kFaceSize/2, face, max_node_level_) {}

void CdlodQuadTree::render(const engine::Camera& cam, QuadGridMesh& mesh,
                           ThreadPool& thread_pool) {
  root_.selectNodes(cam.transform()->pos(), cam.frustum(), mesh, thread_pool);
  root_.age();
}

}  // namespace engine

