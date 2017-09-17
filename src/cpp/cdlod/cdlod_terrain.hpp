// Copyright (c), Tamas Csala

#ifndef ENGINE_CDLOD_TERRAIN_H_
#define ENGINE_CDLOD_TERRAIN_H_

#include <glad/glad.h>
#include <oglwrap/oglwrap.h>
#include <Silice3D/shaders/shader_manager.hpp>
#include <Silice3D/common/thread_pool.hpp>

#include "cdlod/cdlod_quad_tree.hpp"

namespace Cdlod {

class CdlodTerrain {
 public:
  explicit CdlodTerrain(Silice3D::ShaderManager* manager);
  void Setup(const gl::Program& program);
  void Render(const Silice3D::ICamera& cam);

 private:

  QuadGridMesh mesh_;
  CdlodQuadTree faces_[6];
  Silice3D::ThreadPool thread_pool_;
  const gl::Program* program_;
  std::unique_ptr<gl::LazyUniform<glm::vec3>> uCamPos_;
  std::unique_ptr<gl::LazyUniform<GLfloat>> uNodeDimension_;
};

} // namespace Cdlod

#endif
