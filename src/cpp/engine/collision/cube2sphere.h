// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_COLLISION_CUBE2SPHERE_H_
#define ENGINE_COLLISION_CUBE2SPHERE_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "../misc.h"
#include "./bounding_box.h"

namespace engine {

enum class CubeFace {
  kPosX, kNegX, kPosY, kNegY, kPosZ, kNegZ
};

glm::dvec3 Cube2Sphere(const glm::dvec3& pos, CubeFace face, double face_size);
BoundingBox Cube2Sphere(const BoundingBox& bbox, CubeFace face, double face_size);

}


#endif