// Copyright (c), Tamas Csala

#ifndef LOE_SKYBOX_H_
#define LOE_SKYBOX_H_

#include <glad/glad.h>
#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/cube_shape.h>

#include <Silice3D/core/scene.hpp>
#include <Silice3D/shaders/shader_manager.hpp>

class Skybox : public Silice3D::GameObject {
 public:
  explicit Skybox(Silice3D::GameObject* parent);
  virtual ~Skybox() {}

  glm::vec3 GetSunPos() const;
  glm::vec3 GetLightSourcePos() const;

  virtual void Render() override;
  virtual void Update() override;
  virtual void KeyAction(int key, int scancode, int action, int mods) override;

 private:
  float time_;
  float mult_ = 1.0;
  gl::CubeShape cube_;

  Silice3D::ShaderProgram prog_;

  gl::LazyUniform<glm::mat4> uProjectionMatrix_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;
};


#endif  // LOE_SKYBOX_H_
