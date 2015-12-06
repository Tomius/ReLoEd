// Copyright (c) 2015, Tamas Csala

#include "./cdlod_terrain.h"
#include "../oglwrap_all.h"

#define gl(func) OGLWRAP_CHECKED_FUNCTION(func)

namespace engine {

CdlodTerrain::CdlodTerrain(engine::ShaderManager* manager)
    : faces_{
        {Settings::kFaceSize, CubeFace::kPosX},
        {Settings::kFaceSize, CubeFace::kNegX},
        {Settings::kFaceSize, CubeFace::kPosY},
        {Settings::kFaceSize, CubeFace::kNegY},
        {Settings::kFaceSize, CubeFace::kPosZ},
        {Settings::kFaceSize, CubeFace::kNegZ}
      }
    , thread_pool_{4}
{ }

void CdlodTerrain::setup(const gl::Program& program) {
  program_ = &program;

  gl::Use(program);

  mesh_.setupPositions(program | "Terrain_aPosition");
  mesh_.setupRenderData(program | "Terrain_aRenderData");
  mesh_.setupTextureIds(program | "Terrain_aTextureId");
  mesh_.setupTextureInfo(program | "Terrain_aTextureInfo");

  uCamPos_ = engine::make_unique<gl::LazyUniform<glm::vec3>>(
      program, "Terrain_uCamPos");

  gl::Uniform<int>(program, "Terrain_uMaxHeight") =
      Settings::kMaxHeight;

  gl::Uniform<glm::ivec2>(program, "Terrain_uTexSize") =
      glm::ivec2(Settings::kFaceSize, Settings::kFaceSize);

  gl::Uniform<float>(program, "Terrain_uNodeDimension") =
      Settings::kNodeDimension;

  gl::Uniform<float>(program, "Terrain_uLodLevelDistanceMultiplier") =
      Settings::kLodLevelDistanceMultiplier;

  gl::Uniform<int>(program, "Terrain_uMaxLoadLevel") =
      faces_[0].max_node_level();
}

void CdlodTerrain::render(Camera const& cam) {
  if (!uCamPos_) {
    throw std::logic_error("engine::cdlod::terrain requires a setup() call, "
                           "before the use of the render() function.");
  }

  uCamPos_->set(cam.transform()->pos());

  gl::FrontFace(gl::kCcw);
  gl::TemporaryEnable cullface{gl::kCullFace};

  thread_pool_.clear();
  engine::Settings::geom_nodes_count = 0;
  engine::Settings::texture_nodes_count = 0;
  for (int face = 0; face < 6; ++face) {
    mesh_.clearRenderList();
    gl::Uniform<int>(*program_, "Terrain_uFace") = face;
    faces_[face].render(cam, mesh_, thread_pool_);
    mesh_.render();
  }
}

}  // namespace engine

#undef gl
