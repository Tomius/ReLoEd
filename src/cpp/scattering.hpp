// Copyright (c), Tamas Csala

#ifndef LOE_SCATTERING_H_
#define LOE_SCATTERING_H_

#include <glad/glad.h>
#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/rectangle_shape.h>
#include <Silice3D/core/game_object.hpp>
#include <Silice3D/shaders/shader_manager.hpp>

#include "skybox.hpp"

class Scattering : public Silice3D::GameObject {
 public:
  explicit Scattering(Silice3D::GameObject* parent);
  virtual ~Scattering() {}

  glm::vec3 getSunPos() const;
  glm::vec3 getLightSourcePos() const;

  virtual void ScreenResized(size_t w, size_t h) override;
  virtual void Update() override;
  virtual void Render2D() override;

 private:
  gl::RectangleShape rect_;
  Silice3D::ShaderProgram prog_;
  gl::Framebuffer fbo_;
  gl::Renderbuffer depth_buffer_;
  gl::Texture2D color_tex_, depth_tex_;
  gl::LazyUniform<float> uZFar_;
  gl::LazyUniform<glm::vec2> uResolution_;
  gl::LazyUniform<glm::vec3> uCamPos_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;
};


#endif  // LOE_SCATTERING_H_
