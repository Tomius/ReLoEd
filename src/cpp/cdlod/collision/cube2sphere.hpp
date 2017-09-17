// Copyright (c), Tamas Csala

#ifndef ENGINE_COLLISION_CUBE2SPHERE_H_
#define ENGINE_COLLISION_CUBE2SPHERE_H_

#include <Silice3D/common/glm.hpp>

namespace Cdlod {

enum class CubeFace {
  kPosX, kNegX, kPosY, kNegY, kPosZ, kNegZ
};

glm::dvec3 Cube2Sphere(const glm::dvec3& pos, CubeFace face, double kFaceSize);

} // Cdlod


#endif
