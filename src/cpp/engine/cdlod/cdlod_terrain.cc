// Copyright (c) 2015, Tamas Csala

#include "./cdlod_terrain.h"
#include "../oglwrap_all.h"

#define gl(func) OGLWRAP_CHECKED_FUNCTION(func)

namespace engine {

CdlodTerrain::CdlodTerrain(engine::ShaderManager* manager)
    : faces_{
        {GlobalHeightMap::tex_w, GlobalHeightMap::tex_h, CubeFace::kPosX},
        {GlobalHeightMap::tex_w, GlobalHeightMap::tex_h, CubeFace::kNegX},
        {GlobalHeightMap::tex_w, GlobalHeightMap::tex_h, CubeFace::kPosY},
        {GlobalHeightMap::tex_w, GlobalHeightMap::tex_h, CubeFace::kNegY},
        {GlobalHeightMap::tex_w, GlobalHeightMap::tex_h, CubeFace::kPosZ},
        {GlobalHeightMap::tex_w, GlobalHeightMap::tex_h, CubeFace::kNegZ}
      }
{ }

void CdlodTerrain::setup(const gl::Program& program) {
  program_ = &program;

  gl::Use(program);

  mesh_.setupPositions(program | "Terrain_aPosition");
  mesh_.setupRenderData(program | "Terrain_aRenderData");

  uCamPos_ = engine::make_unique<gl::LazyUniform<glm::vec3>>(
      program, "Terrain_uCamPos");

  gl::Uniform<int>(program, "Terrain_max_height") =
      GlobalHeightMap::max_height;

  gl::Uniform<glm::ivec2>(program, "Terrain_uTexSize") =
      glm::ivec2(GlobalHeightMap::tex_w, GlobalHeightMap::tex_h);

  gl::Uniform<float>(program, "Terrain_uNodeDimension") =
      GlobalHeightMap::node_dimension;

  gl::Uniform<float>(program, "Terrain_uLodLevelDistanceMultiplier") =
      GlobalHeightMap::lod_level_distance_multiplier;
}

void CdlodTerrain::render(Camera const& cam) {
  if (!uCamPos_) {
    throw std::logic_error("engine::cdlod::terrain requires a setup() call, "
                           "before the use of the render() function.");
  }

  uCamPos_->set(cam.transform()->pos());

  gl::FrontFace(gl::kCcw);
  gl::TemporaryEnable cullface{gl::kCullFace};

  for (int face = 0; face < 6; ++face) {
    if (face % 2 == 0) {
      gl::FrontFace(gl::kCw);
    } else {
      gl::FrontFace(gl::kCcw);
    }
    mesh_.clearRenderList();
    gl::Uniform<int>(*program_, "Terrain_uFace") = face;
    faces_[face].render(cam, mesh_);
    mesh_.render();
  }
}

}  // namespace engine

#undef gl
