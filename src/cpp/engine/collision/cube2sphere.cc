// Copyright (c) 2015, Tamas Csala

#include "./cube2sphere.h"
#include "../settings.h"

namespace engine {

static glm::dvec3 Spherify(const glm::dvec3& p) {
  return {
    p.x * sqrt(1 - sqr(p.y)/2 - sqr(p.z)/2 + sqr(p.y*p.z)/3),
    p.y * sqrt(1 - sqr(p.z)/2 - sqr(p.x)/2 + sqr(p.z*p.x)/3),
    p.z * sqrt(1 - sqr(p.x)/2 - sqr(p.y)/2 + sqr(p.x*p.y)/3)
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
  }
}

static BoundingBox FaceLocalToUnitCube(const BoundingBox& bbox,
                                       CubeFace face,
                                       double kFaceSize) {
  glm::dvec3 a = FaceLocalToUnitCube(bbox.mins(), face, kFaceSize);
  glm::dvec3 b = FaceLocalToUnitCube(bbox.maxes(), face, kFaceSize);
  // values might change sign in the mapping
  return BoundingBox{glm::min(a, b), glm::max(a, b)};
}

glm::dvec3 Cube2Sphere(const glm::dvec3& pos,
                               CubeFace face,
                               double kFaceSize) {
  glm::dvec3 pos_on_cube = FaceLocalToUnitCube(pos, face, kFaceSize);
  return (Settings::kSphereRadius + pos.y) * Spherify(pos_on_cube);
}

BoundingBox Cube2Sphere(const BoundingBox& bbox,
                                        CubeFace face, double kFaceSize) {
  using namespace glm;

  BoundingBox bbox_on_cube = FaceLocalToUnitCube(bbox, face, kFaceSize);
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

  double min_radius = Settings::kSphereRadius + bbox.mins().y;
  double max_radius = Settings::kSphereRadius + bbox.maxes().y;

  return {
    glm::min(min_radius * min_on_unit_sphere, max_radius * min_on_unit_sphere),
    glm::max(min_radius * max_on_unit_sphere, max_radius * max_on_unit_sphere)
  };
}

BoundingBox Plane2Cube(const BoundingBox& bbox,
                       CubeFace face,
                       double kFaceSize) {
  double min_cube_size = Settings::kSphereRadius + bbox.mins().y;
  double max_cube_size = Settings::kSphereRadius + bbox.maxes().y;

  glm::dvec3 candidates[4];
  glm::dvec3 a = FaceLocalToUnitCube(bbox.mins(), face, kFaceSize);
  candidates[0] = a * min_cube_size;
  candidates[1] = a * max_cube_size;

  glm::dvec3 b = FaceLocalToUnitCube(bbox.maxes(), face, kFaceSize);
  candidates[2] = b * min_cube_size;
  candidates[3] = b * max_cube_size;

  glm::dvec3 min = candidates[0], max = candidates[0];
  for (int i = 1; i < 4; ++i) {
    min = glm::min(min, candidates[i]);
    max = glm::max(max, candidates[i]);
  }

  // values might change sign in the mapping
  return BoundingBox{min, max};
}

static const double isqrt2 = 0.70710676908493042;

static glm::dvec3 Cubify(const glm::dvec3& s) {
  double xx2 = s.x * s.x * 2.0;
  double yy2 = s.y * s.y * 2.0;

  glm::dvec2 v = glm::dvec2(xx2 - yy2, yy2 - xx2);

  double ii = v.y - 3.0;
  ii *= ii;

  double isqrt = -sqrt(ii - 12.0 * xx2) + 3.0;

  v = glm::sqrt(v + isqrt);
  v *= isqrt2;

  return glm::sign(s) * glm::dvec3(v, 1.0);
}

inline glm::dvec3 XZY(const glm::dvec3& v) {
  return {v.x, v.z, v.y};
}

inline glm::dvec3 YZX(const glm::dvec3& v) {
  return {v.y, v.z, v.x};
}

inline glm::dvec3 ZXY(const glm::dvec3& v) {
  return {v.z, v.x, v.y};
}

glm::dvec3 Sphere2Cube(const glm::dvec3& sphere) {
  glm::dvec3 f = glm::abs(sphere);

  bool a = f.y >= f.x && f.y >= f.z;
  bool b = f.x >= f.z;

  return a ? XZY(Cubify(XZY(sphere)))
        : b ? ZXY(Cubify(YZX(sphere)))
        : Cubify(sphere);
}

}
