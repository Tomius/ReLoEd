// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_QUAD_TREE_NODE_H_

#include <memory>
#include "geometry/quad_grid_mesh.h"
#include "./texture_info.h"
#include "../settings.h"
#include "../collision/spherized_aabb.h"
#include "../thread_pool.h"

namespace engine {

class CdlodQuadTreeNode {
 public:
  CdlodQuadTreeNode(double x, double z, CubeFace face, int level,
                    CdlodQuadTreeNode* parent = nullptr);
  ~CdlodQuadTreeNode();

  void age();
  void selectNodes(const glm::vec3& cam_pos,
                   const Frustum& frustum,
                   QuadGridMesh& grid_mesh,
                   ThreadPool& thread_pool);

  void selectTexture(const glm::vec3& cam_pos,
                     const Frustum& frustum,
                     ThreadPool& thread_pool,
                     StreamedTextureInfo& texinfo,
                     int recursion_level = 0);

 private:
  double x_, z_;
  CubeFace face_;
  int level_;
  SpherizedAABB bbox_;
  CdlodQuadTreeNode* parent_;
  std::unique_ptr<CdlodQuadTreeNode> children_[4];
  int last_used_ = 0;

  TextureInfo texture_;

  // If a node is not used for this much time (frames), it will be unloaded.
  static const int kTimeToLiveInMemory = 1 << 4;

  // --- functions ---

  double scale() const { return pow(2, level_); }
  double size() { return Settings::kNodeDimension * scale(); }

  bool collidesWithSphere(const Sphere& sphere) const {
    return bbox_.collidesWithSphere(sphere);
  }

  void initChild(int i);
  std::string getHeightMapPath() const;
  std::string getDiffuseMapPath() const;

  int elevationTextureLevel() const;
  int diffuseTextureLevel() const;

  bool hasElevationTexture() const;
  bool hasDiffuseTexture() const;

  void loadTexture(bool synchronous_load);
  void upload();
};

}

#endif
