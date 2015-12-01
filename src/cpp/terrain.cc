// Copyright (c) 2015, Tamas Csala

#include "./terrain.h"
#include <string>

#include "engine/scene.h"

Terrain::Terrain(engine::GameObject* parent)
    : engine::GameObject(parent)
    , mesh_(scene_->shader_manager())
    , prog_(scene_->shader_manager()->get("terrain.vert"),
            scene_->shader_manager()->get("terrain.frag"))
    , uDepthCoef_(prog_, "uDepthCoef")
    , uProjectionMatrix_(prog_, "uProjectionMatrix")
    , uCameraMatrix_(prog_, "uCameraMatrix")
    , uModelMatrix_(prog_, "uModelMatrix") {
  gl::Use(prog_);
  mesh_.setup(prog_);

  prog_.validate();
}

void Terrain::render() {
  const engine::Camera& cam = *scene_->camera();

  gl::Use(prog_);
  prog_.update();
  uCameraMatrix_ = cam.cameraMatrix();
  uProjectionMatrix_ = cam.projectionMatrix();
  uModelMatrix_ = transform()->matrix();
  uDepthCoef_ = 2.0 / log2(cam.z_far() + 1.0);

  mesh_.render(cam);
}


