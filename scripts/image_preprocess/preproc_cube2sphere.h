#pragma once

#include "./preproc_settings.h"

enum CubeFace {
  kPosX, kNegX, kPosY, kNegY, kPosZ, kNegZ
};

inline glm::dvec3 Cubify(const glm::dvec3& p) {
  return {
    p.x * sqrt(1 - sqr(p.y)/2 - sqr(p.z)/2 + sqr(p.y*p.z)/3),
    p.y * sqrt(1 - sqr(p.z)/2 - sqr(p.x)/2 + sqr(p.z*p.x)/3),
    p.z * sqrt(1 - sqr(p.x)/2 - sqr(p.y)/2 + sqr(p.x*p.y)/3)
  };
}

inline double sgn(double x) {
  return x > 0 ? 1 : -1;
}

inline void HandleBorders(glm::dvec3& pos,
                          CubeFace face) {
  glm::dvec3 abspos = glm::abs(pos);

  if (abspos.x > 1) {
    assert(abspos.x < 2);
    pos.x = sgn(pos.x) * 1;

    switch (face) {
      case CubeFace::kPosX: case CubeFace::kNegX:
        assert(false);
      case CubeFace::kPosY: case CubeFace::kNegY:
        pos.y -= sgn(pos.y) * (abspos.x - 1);
        break;
      case CubeFace::kPosZ: case CubeFace::kNegZ:
        pos.z -= sgn(pos.z) * (abspos.x - 1);
        break;
    }
  }

  if (abspos.y > 1) {
    assert(abspos.y < 2);
    pos.y = sgn(pos.y) * 1;

    switch (face) {
      case CubeFace::kPosX: case CubeFace::kNegX:
        pos.x -= sgn(pos.x) * (abspos.y - 1);
        break;
      case CubeFace::kPosY: case CubeFace::kNegY:
        assert(false);
      case CubeFace::kPosZ: case CubeFace::kNegZ:
        pos.z -= sgn(pos.z) * (abspos.y - 1);
        break;
    }
  }

  if (abspos.z > 1) {
    assert(abspos.z < 2);
    pos.z = sgn(pos.z) * 1;

    switch (face) {
      case CubeFace::kPosX: case CubeFace::kNegX:
        pos.x -= sgn(pos.x) * (abspos.z - 1);
        break;
      case CubeFace::kPosY: case CubeFace::kNegY:
        pos.y -= sgn(pos.y) * (abspos.z - 1);
        break;
      case CubeFace::kPosZ: case CubeFace::kNegZ:
        assert(false);
    }
  }
}

inline glm::dvec3 FaceLocalToUnitCube2(const glm::dvec3& pos,
                                       CubeFace face,
                                       double face_size) {
  glm::dvec3 no_height = glm::dvec3(pos.x, 0, pos.z);
  glm::dvec3 n = (no_height - face_size/2) / (face_size/2); // normalized to [-1, 1]
  switch (face) {
    case CubeFace::kPosX: return {+n.y, +n.z, -n.x}; break;
    case CubeFace::kNegX: return {-n.y, +n.z, +n.x}; break;
    case CubeFace::kPosY: return {+n.z, +n.y, +n.x}; break;
    case CubeFace::kNegY: return {-n.z, -n.y, +n.x}; break;
    case CubeFace::kPosZ: return {+n.x, +n.z, +n.y}; break;
    case CubeFace::kNegZ: return {-n.x, +n.z, -n.y}; break;
  }
}

inline glm::dvec3 FaceLocalToUnitCube(const glm::dvec3& pos,
                                      CubeFace face,
                                      double face_size) {
  glm::dvec3 ret = FaceLocalToUnitCube2(pos, face, face_size);
  HandleBorders(ret, face);
  return ret;
}

inline glm::dvec3 Cube2Sphere(const glm::dvec3& pos,
                              CubeFace face,
                              double face_size) {
  glm::dvec3 pos_on_cube = FaceLocalToUnitCube(pos, face, face_size);
  return (kRadius + pos.y) * Cubify(pos_on_cube);
}

inline glm::dvec2 Cube2Plane(const glm::dvec3& pos,
                              CubeFace face,
                              double face_size) {
  glm::dvec3 pos_on_cube = FaceLocalToUnitCube(pos, face, face_size);
  glm::dvec3 pos_on_unit_sphere = Cubify(pos_on_cube);
  glm::dvec2 angles{
    atan2(pos_on_unit_sphere.z, pos_on_unit_sphere.x),
    acos(pos_on_unit_sphere.y)
  };

  return glm::dvec2{
    (angles.x + M_PI) / (2*M_PI) * (kInputWidth - 1),
    (M_PI - angles.y) / M_PI * (kInputHeight - 1),
  };
}
