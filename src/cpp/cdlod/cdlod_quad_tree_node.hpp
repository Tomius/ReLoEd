// Copyright (c), Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_QUAD_TREE_NODE_H_

#include <memory>
#include <Silice3D/common/thread_pool.hpp>

#include "cdlod/geometry/quad_grid_mesh.hpp"
#include "cdlod/texture_info.hpp"
#include "cdlod/cdlod_terrain_settings.hpp"
#include "cdlod/collision/spherized_aabb.hpp"

namespace Cdlod {

class CdlodQuadTreeNode {
 public:
  CdlodQuadTreeNode(double x, double z, CubeFace face, int level,
                    CdlodQuadTreeNode* parent = nullptr);
  ~CdlodQuadTreeNode();

  CdlodQuadTreeNode(CdlodQuadTreeNode&&) = default;

  void age();
  void selectNodes(const glm::vec3& cam_pos,
                   const Silice3D::Frustum& frustum,
                   QuadGridMesh& grid_mesh,
                   Silice3D::ThreadPool& thread_pool);

  void selectTexture(const glm::vec3& cam_pos,
                     const Silice3D::Frustum& frustum,
                     Silice3D::ThreadPool& thread_pool,
                     StreamedTextureInfo& texinfo,
                     bool is_node_visible,
                     int recursion_level = 0);

 private:
  double x_, z_;
  CubeFace face_;
  int level_;
  SpherizedAABBDivided bbox_;
  CdlodQuadTreeNode* parent_;
  std::unique_ptr<CdlodQuadTreeNode> children_[4];
  int last_used_ = 0;
  bool is_enqued_for_async_load_ = false;

  TextureInfo texture_;

  // If a node is not used for this much time (frames), it will be unloaded.
  static constexpr int kTimeToLiveInMemory = 1 << 12;

  // --- functions ---

  double scale() const { return pow(2, level_); }
  double size() { return CdlodTerrainSettings::kNodeDimension * scale(); }

  bool collidesWithSphere(const Silice3D::Sphere& sphere) const;

  void initChild(int i);
  std::string getHeightMapPath() const;
  std::string getDiffuseMapPath() const;

  int elevationTextureLevel() const;
  int diffuseTextureLevel() const;

  bool hasElevationTexture() const;
  bool hasDiffuseTexture() const;

  void loadTexture(bool synchronous_load);
  void upload();
  void calculateMinMax();
  void refreshMinMax();
};

} // namespace Cdlod

#endif
