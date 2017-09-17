// Copyright (c), Tamas Csala

#ifndef LOE_TERRAIN_H_
#define LOE_TERRAIN_H_

#include <glad/glad.h>
#include <oglwrap/oglwrap.h>
#include <Silice3D/core/game_object.hpp>
#include <Silice3D/shaders/shader_manager.hpp>

#include "cdlod/cdlod_terrain.hpp"

class Terrain : public Silice3D::GameObject {
 public:
  explicit Terrain(Silice3D::GameObject* parent);
  virtual ~Terrain() {}

 private:
  Cdlod::CdlodTerrain mesh_;
  Silice3D::ShaderProgram prog_;  // has to be inited after mesh_

  gl::LazyUniform<float> uDepthCoef_;
  gl::LazyUniform<glm::mat4> uProjectionMatrix_, uCameraMatrix_, uModelMatrix_;

  virtual void Render() override;
};

#endif  // LOE_TERRAIN_H_
