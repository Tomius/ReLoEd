// Copyright (c), Tamas Csala

#include <Silice3D/common/math.hpp>

#include "cdlod/collision/cube2sphere.hpp"
#include "cdlod/cdlod_terrain_settings.hpp"

namespace Cdlod {

static glm::dvec3 Cubify(const glm::dvec3& p) {
  using Silice3D::Math::Sqr;

  return {
    p.x * sqrt(1 - Sqr(p.y)/2 - Sqr(p.z)/2 + Sqr(p.y*p.z)/3),
    p.y * sqrt(1 - Sqr(p.z)/2 - Sqr(p.x)/2 + Sqr(p.z*p.x)/3),
    p.z * sqrt(1 - Sqr(p.x)/2 - Sqr(p.y)/2 + Sqr(p.x*p.y)/3)
  };
}

static glm::dvec3 FaceLocalToUnitCube(const glm::dvec3& pos,
                                      CubeFace face,
                                      double kFaceSize) {
  glm::dvec3 no_height = glm::dvec3(pos.x, 0, pos.z);
  glm::dvec3 n = (no_height - kFaceSize/2) / (kFaceSize/2); // normalized to [-1, 1]
  switch (face) {
    case CubeFace::kPosX: return {-n.y, -n.z, -n.x}; break;
    case CubeFace::kNegX: return {+n.y, -n.z, +n.x}; break;
    case CubeFace::kPosY: return {-n.z, -n.y, +n.x}; break;
    case CubeFace::kNegY: return {+n.z, +n.y, +n.x}; break;
    case CubeFace::kPosZ: return {-n.x, -n.z, +n.y}; break;
    case CubeFace::kNegZ: return {+n.x, -n.z, -n.y}; break;
    default:
      throw std::runtime_error("Invalid CubeFace enum");
  }
}

} // namespace Cdlod

glm::dvec3 Cdlod::Cube2Sphere(const glm::dvec3& pos,
                              CubeFace face,
                              double kFaceSize) {
  glm::dvec3 pos_on_cube = FaceLocalToUnitCube(pos, face, kFaceSize);
  return (CdlodTerrainSettings::kSphereRadius + pos.y) * Cubify(pos_on_cube);
}

