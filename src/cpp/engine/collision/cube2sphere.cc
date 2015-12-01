// Copyright (c) 2015, Tamas Csala

#include "./cube2sphere.h"
#include "../global_height_map.h"

namespace engine {

static glm::dvec3 Cubify(const glm::dvec3& p) {
  return {
    p.x * sqrt(1 - sqr(p.y)/2 - sqr(p.z)/2 + sqr(p.y*p.z)/3),
    p.y * sqrt(1 - sqr(p.z)/2 - sqr(p.x)/2 + sqr(p.z*p.x)/3),
    p.z * sqrt(1 - sqr(p.x)/2 - sqr(p.y)/2 + sqr(p.x*p.y)/3)
  };
}

static glm::dvec3 FaceLocalToUnitCube(const glm::dvec3& pos,
                                      CubeFace face,
                                      double face_size) {
  glm::dvec3 no_height = glm::dvec3(pos.x, 0, pos.z);
  glm::dvec3 n = (no_height - face_size/2) / (face_size/2); // normalized to [-1, 1]
  switch (face) {
    case CubeFace::kPosX: return {+n.z, +n.x, +n.y}; break;
    case CubeFace::kNegX: return {+n.z, +n.x, -n.y}; break;
    case CubeFace::kPosY: return {+n.x, +n.y, +n.z}; break;
    case CubeFace::kNegY: return {+n.x, -n.y, +n.z}; break;
    case CubeFace::kPosZ: return {+n.y, +n.z, +n.x}; break;
    case CubeFace::kNegZ: return {-n.y, +n.z, +n.x}; break;
  }
}

static BoundingBox FaceLocalToUnitCube(const BoundingBox& bbox,
                                       CubeFace face,
                                       double face_size) {
  glm::dvec3 a = FaceLocalToUnitCube(bbox.mins(), face, face_size);
  glm::dvec3 b = FaceLocalToUnitCube(bbox.maxes(), face, face_size);
  // values might change sign in the mapping
  return BoundingBox{glm::min(a, b), glm::max(a, b)};
}

}

glm::dvec3 engine::Cube2Sphere(const glm::dvec3& pos,
                               CubeFace face,
                               double face_size) {
  glm::dvec3 pos_on_cube = FaceLocalToUnitCube(pos, face, face_size);
  return (GlobalHeightMap::sphere_radius + pos.y) * Cubify(pos_on_cube);
}

engine::BoundingBox engine::Cube2Sphere(const engine::BoundingBox& bbox,
                                        CubeFace face, double face_size) {
  using namespace glm;

  // bbox_on_cube
  BoundingBox bbox_on_cube = FaceLocalToUnitCube(bbox, face, face_size);

  dvec3 mins = bbox_on_cube.mins();
  dvec3 maxes = bbox_on_cube.maxes();

  // the first minus part in the sqrt
  dvec3 a = sqr(dvec3{mins.y, mins.z, mins.x});
  dvec3 b = sqr(dvec3{maxes.y, maxes.z, maxes.x});
  dvec3 first_sq_min = glm::min(a, b);
  dvec3 first_sq_max = glm::max(a, b);

  a = sqr(dvec3{mins.z, mins.x, mins.y});
  b = sqr(dvec3{maxes.z, maxes.x, maxes.y});
  dvec3 second_sq_min = glm::min(a, b);
  dvec3 second_sq_max = glm::max(a, b);

  dvec3 cross_sq_min = first_sq_min*second_sq_min;
  dvec3 cross_sq_max = first_sq_max*second_sq_max;

  dvec3 sqrt_min = 1.0 - first_sq_max/2.0 - second_sq_max/2.0 + cross_sq_min/3.0;
  sqrt_min = sqrt(clamp(sqrt_min, dvec3(0.0), dvec3(1.0)));

  dvec3 sqrt_max = 1.0 - first_sq_min/2.0 - second_sq_min/2.0 + cross_sq_max/3.0;
  sqrt_max = sqrt(clamp(sqrt_max, dvec3(0.0), dvec3(1.0)));

  a = mins * sqrt_min;
  b = mins * sqrt_max;
  dvec3 min_on_unit_sphere = glm::min(a, b);

  a = maxes * sqrt_min;
  b = maxes * sqrt_max;
  dvec3 max_on_unit_sphere = glm::max(a, b);

  double min_radius = GlobalHeightMap::sphere_radius + bbox.mins().y;
  double max_radius = GlobalHeightMap::sphere_radius + bbox.maxes().y;

  return {
    glm::min(min_radius * min_on_unit_sphere, max_radius * min_on_unit_sphere),
    glm::max(min_radius * max_on_unit_sphere, max_radius * max_on_unit_sphere)
  };
}
