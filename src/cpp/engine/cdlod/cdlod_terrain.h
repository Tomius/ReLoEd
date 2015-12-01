// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TERRAIN_H_
#define ENGINE_CDLOD_TERRAIN_H_

#include "./cdlod_quad_tree.h"
#include "../oglwrap_all.h"
#include "../shader_manager.h"

namespace engine {

class CdlodTerrain {
 public:
  explicit CdlodTerrain(engine::ShaderManager* manager);
  void setup(const gl::Program& program);
  void render(const Camera& cam);

 private:
  QuadGridMesh mesh_;
  CdlodQuadTree faces_[6];
  const gl::Program* program_;
  std::unique_ptr<gl::LazyUniform<glm::vec3>> uCamPos_;
  std::unique_ptr<gl::LazyUniform<GLfloat>> uNodeDimension_;
};

}  // namespace engine

#endif
