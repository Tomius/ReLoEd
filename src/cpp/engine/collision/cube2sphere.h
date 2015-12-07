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

glm::dvec3 Cube2Sphere(const glm::dvec3& pos, CubeFace face, double kFaceSize);
BoundingBox Cube2Sphere(const BoundingBox& bbox, CubeFace face, double kFaceSize);

glm::dvec3 Sphere2Cube(const glm::dvec3& pos);
BoundingBox Plane2Cube(const BoundingBox& bbox, CubeFace face, double kFaceSize);

}


#endif
