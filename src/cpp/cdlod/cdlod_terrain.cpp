// Copyright (c), Tamas Csala

#include <Silice3D/common/make_unique.hpp>

#include "cdlod/cdlod_terrain.hpp"
#include "cdlod/cdlod_terrain_settings.hpp"

namespace Cdlod {

CdlodTerrain::CdlodTerrain(Silice3D::ShaderManager* manager)
    : faces_{
        {CdlodTerrainSettings::kFaceSize, CubeFace::kPosX},
        {CdlodTerrainSettings::kFaceSize, CubeFace::kNegX},
        {CdlodTerrainSettings::kFaceSize, CubeFace::kPosY},
        {CdlodTerrainSettings::kFaceSize, CubeFace::kNegY},
        {CdlodTerrainSettings::kFaceSize, CubeFace::kPosZ},
        {CdlodTerrainSettings::kFaceSize, CubeFace::kNegZ}
      }
    , thread_pool_{4}
{ }

void CdlodTerrain::Setup(const gl::Program& program) {
  program_ = &program;

  gl::Use(program);

  mesh_.setupPositions(program | "Terrain_aPosition");
  mesh_.setupRenderData(program | "Terrain_aRenderData");

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

  mesh_.setupCurrentDiffuseTextureIds(
      program | "Terrain_aCurrentDiffuseTextureId");
  mesh_.setupCurrentDiffuseTexturePosAndSize(
      program | "Terrain_aCurrentDiffuseTexturePosAndSize");
  mesh_.setupNextDiffuseTextureIds(
      program | "Terrain_aNextDiffuseTextureId");
  mesh_.setupNextDiffuseTexturePosAndSize(
      program | "Terrain_aNextDiffuseTexturePosAndSize");

  uCamPos_ = Silice3D::make_unique<gl::LazyUniform<glm::vec3>>(
      program, "Terrain_uCamPos");

  gl::Uniform<int>(program, "Terrain_uMaxHeight") =
      int(CdlodTerrainSettings::kMaxHeight);

  gl::Uniform<glm::ivec2>(program, "Terrain_uTexSize") =
      glm::ivec2(CdlodTerrainSettings::kFaceSize, CdlodTerrainSettings::kFaceSize);

  gl::Uniform<float>(program, "Terrain_uSmallestGeometryLodDistance") =
      float(CdlodTerrainSettings::kSmallestGeometryLodDistance);

  gl::Uniform<float>(program, "Terrain_uSmallestTextureLodDistance") =
      float(CdlodTerrainSettings::kSmallestTextureLodDistance);

  gl::Uniform<int>(program, "Terrain_uLevelOffset") =
      int(CdlodTerrainSettings::kLevelOffset);

  gl::Uniform<int>(program, "Terrain_uMaxLoadLevel") =
      faces_[0].max_node_level();

  gl::Uniform<int>(program, "Terrain_uTextureDimension") =
      int(CdlodTerrainSettings::kTextureDimension);

  gl::Uniform<int>(program, "Terrain_uTextureDimensionWBorders") =
      int(CdlodTerrainSettings::kElevationTexSizeWithBorders);
}

void CdlodTerrain::Render(const Silice3D::ICamera& cam) {
  if (!uCamPos_) {
    throw std::logic_error("Silice3D::CdlodTerrain requires a Setup() call, "
                           "before the use of the Render() function.");
  }

  uCamPos_->set(cam.transform().pos());

  gl::FrontFace(gl::kCcw);
  gl::TemporaryEnable cullface{gl::kCullFace};

  thread_pool_.clear();
  CdlodTerrainSettings::geom_nodes_count = 0;
  if (CdlodTerrainSettings::update) {
    mesh_.clearRenderList();
    for (int face = 0; face < 6; ++face) {
      faces_[face].render(cam, mesh_, thread_pool_);
    }
  }
  if (CdlodTerrainSettings::render) {
    mesh_.render();
  }
}

} // namespace Cdlod
