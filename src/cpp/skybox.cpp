// Copyright (c), Tamas Csala

#include <Silice3D/core/game_engine.hpp>
#include <Silice3D/camera/perspective_camera.hpp>

#include "skybox.hpp"

const float day_duration = 256.0f, day_start = 0;

Skybox::Skybox(Silice3D::GameObject* parent)
    : Silice3D::GameObject(parent)
    , time_(day_start)
    , cube_({gl::CubeShape::kPosition})
    , prog_(scene_->shader_manager()->get("skybox.vert"),
            scene_->shader_manager()->get("skybox.frag"))
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix") {
  Silice3D::ShaderFile *sky_fs = scene_->shader_manager()->get("sky.frag");
  sky_fs->set_update_func([this](const gl::Program& prog) {
    gl::Uniform<glm::vec3>(prog, "uSunPos") = GetSunPos();
  });

  gl::Use(prog_);
  prog_.validate();
  (prog_ | "aPosition").bindLocation(cube_.kPosition);
}

glm::vec3 Skybox::GetSunPos() const {
  return glm::normalize(
           glm::vec3{-cos(time_ * 2 * M_PI / day_duration), 0.1f,
                     -sin(time_ * 2 * M_PI / day_duration)});
}

glm::vec3 Skybox::GetLightSourcePos() const {
  glm::vec3 sun_pos = GetSunPos();
  return sun_pos.y > 0 ? sun_pos : -sun_pos;
}

void Skybox::Update() {
  time_ += scene_->environment_time().dt() * mult_;
}

void Skybox::KeyAction(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_KP_ADD) {
      mult_ = 16.0;
    } else if (key == GLFW_KEY_KP_SUBTRACT) {
      mult_ = -16.0;
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_KP_SUBTRACT) {
      mult_ = 1.0;
    }
  }
}

void Skybox::Render() {
  auto cam = dynamic_cast<Silice3D::PerspectiveCamera*>(scene_->camera());

  gl::Use(prog_);
  prog_.update();
  uCameraMatrix_ = glm::mat3(cam->cameraMatrix());
  uProjectionMatrix_ = cam->projectionMatrix();
  gl::Uniform<float>(prog_, "uZFar") = cam->z_far();

  gl::TemporaryDisable depth_test{gl::kDepthTest};

  gl::DepthMask(false);
  cube_.render();
  gl::DepthMask(true);
}
