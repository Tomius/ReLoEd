// Copyright (c), Tamas Csala

#include <memory>
#include <Silice3D/camera/icamera.hpp>

#include "cdlod/cdlod_quad_tree.hpp"
#include "cdlod/geometry/quad_grid_mesh.hpp"
#include "cdlod/cdlod_quad_tree_node.hpp"
#include "cdlod/cdlod_terrain_settings.hpp"

namespace Cdlod {

CdlodQuadTree::CdlodQuadTree(size_t kFaceSize, CubeFace face)
  : max_node_level_(log2(kFaceSize) - CdlodTerrainSettings::kNodeDimensionExp)
  , root_(kFaceSize/2, kFaceSize/2, face, max_node_level_) {}

void CdlodQuadTree::render(const Silice3D::ICamera& cam, QuadGridMesh& mesh,
                           Silice3D::ThreadPool& thread_pool) {
  root_.selectNodes(cam.transform().pos(), cam.frustum(), mesh, thread_pool);
  root_.age();
}

}  // namespace Cdlod

