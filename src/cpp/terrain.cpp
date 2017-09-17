// Copyright (c), Tamas Csala

#include <string>
#include <Silice3D/core/scene.hpp>
#include <Silice3D/camera/perspective_camera.hpp>

#include "terrain.hpp"

Terrain::Terrain(Silice3D::GameObject* parent)
    : Silice3D::GameObject(parent)
    , mesh_(scene_->shader_manager())
    , prog_(scene_->shader_manager()->get("terrain.vert"),
            scene_->shader_manager()->get("terrain.frag"))
    , uDepthCoef_(prog_, "uDepthCoef")
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix") {
  gl::Use(prog_);
  mesh_.Setup(prog_);

  prog_.validate();
}

void Terrain::Render() {
  auto cam = dynamic_cast<Silice3D::PerspectiveCamera*>(scene_->camera());

  gl::Use(prog_);
  prog_.update();
  uCameraMatrix_ = cam->cameraMatrix();
  uProjectionMatrix_ = cam->projectionMatrix();
  uModelMatrix_ = transform().matrix();
  uDepthCoef_ = 2.0 / log2(cam->z_far() + 1.0);

  mesh_.Render(*cam);
}


