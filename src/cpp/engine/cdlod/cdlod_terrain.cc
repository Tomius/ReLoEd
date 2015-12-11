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
    , thread_pool_{1}
{ }

void CdlodTerrain::setup(const gl::Program& program) {
  program_ = &program;

  gl::Use(program);

  mesh_.setupPositions(program | "Terrain_aPosition");
  mesh_.setupRenderData(program | "Terrain_aRenderData");
  mesh_.setupMinMax(program | "Terrain_aMinMax");

  mesh_.setupCurrentGeometryTextureIds(
      program | "Terrain_aCurrentGeometryTextureId");
  mesh_.setupCurrentGeometryTexturePosAndSize(
      program | "Terrain_aCurrentGeometryTexturePosAndSize");
  mesh_.setupNextGeometryTextureIds(
      program | "Terrain_aNextGeometryTextureId");
  mesh_.setupNextGeometryTexturePosAndSize(
      program | "Terrain_aNextGeometryTexturePosAndSize");

  mesh_.setupCurrentNormalTextureIds(
      program | "Terrain_aCurrentNormalTextureId");
  mesh_.setupCurrentNormalTexturePosAndSize(
      program | "Terrain_aCurrentNormalTexturePosAndSize");
  mesh_.setupNextNormalTextureIds(
      program | "Terrain_aNextNormalTextureId");
  mesh_.setupNextNormalTexturePosAndSize(
      program | "Terrain_aNextNormalTexturePosAndSize");

  uCamPos_ = engine::make_unique<gl::LazyUniform<glm::vec3>>(
      program, "Terrain_uCamPos");

  gl::Uniform<int>(program, "Terrain_uMaxHeight") =
      Settings::kMaxHeight;

  gl::Uniform<glm::ivec2>(program, "Terrain_uTexSize") =
      glm::ivec2(Settings::kFaceSize, Settings::kFaceSize);

  gl::Uniform<float>(program, "Terrain_uSmallestGeometryLodDistance") =
      Settings::kSmallestGeometryLodDistance;

  gl::Uniform<float>(program, "Terrain_uSmallestTextureLodDistance") =
      Settings::kSmallestTextureLodDistance;

  gl::Uniform<int>(program, "Terrain_uMaxLoadLevel") =
      faces_[0].max_node_level();

  gl::Uniform<int>(program, "Terrain_uTextureDimension") =
      Settings::kTextureDimension;

  gl::Uniform<int>(program, "Terrain_uTextureDimensionWBorders") =
      Settings::kTextureDimension + 2*Settings::kTextureBorderSize;
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
  if (Settings::update) {
    mesh_.clearRenderList();
    for (int face = 0; face < 6; ++face) {
      faces_[face].render(cam, mesh_, thread_pool_);
    }
  }
  if (Settings::render) {
    mesh_.render();
  }
}

}  // namespace engine

#undef gl
