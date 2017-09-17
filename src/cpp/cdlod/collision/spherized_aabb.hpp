// Copyright (c), Tamas Csala

#ifndef ENGINE_COLLISION_SPHERIZED_AABB_H_
#define ENGINE_COLLISION_SPHERIZED_AABB_H_

#include <Silice3D/common/math.hpp>
#include <Silice3D/collision/sphere.hpp>
#include <Silice3D/collision/frustum.hpp>

#include "cdlod/collision/cube2sphere.hpp"

namespace Cdlod {

class SpherizedAABB {
 public:
  SpherizedAABB() = default;
  SpherizedAABB(const glm::dvec3& mins, const glm::dvec3& maxes,
                CubeFace face, double face_size);

  bool collidesWithSphere(const Silice3D::Sphere& sphere) const;
  bool collidesWithFrustum(const Silice3D::Frustum& frustum) const;

private:
  struct Interval {
    double min;
    double max;
  };

  Silice3D::Sphere bsphere_;

  glm::dvec3 normals_[4];
  Interval extents_[4];
  Interval radial_extent_;

  static glm::dvec3 GetNormal(glm::dvec3 vertices[], int a, int b, int c, int d);
  static bool HasIntersection(const Interval& a, const Interval& b);
  static Interval getExtent(const glm::dvec3& normal,
                            const glm::dvec3& m_space_min,
                            const glm::dvec3& m_space_max,
                            CubeFace face, double face_size);
};

constexpr int kAabbSubdivisionRate = 2;

class SpherizedAABBDivided {
public:
  SpherizedAABBDivided() = default;
  SpherizedAABBDivided(const glm::dvec3& mins, const glm::dvec3& maxes,
                       CubeFace face, double face_size);

  bool collidesWithSphere(const Silice3D::Sphere& sphere) const;
  bool collidesWithFrustum(const Silice3D::Frustum& frustum) const;

private:
  SpherizedAABB main_;
  SpherizedAABB subs_[Silice3D::Math::Cube(kAabbSubdivisionRate)];
};

} // namespace Cdlod

#endif
