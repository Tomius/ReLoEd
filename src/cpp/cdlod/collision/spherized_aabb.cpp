// Copyright (c), Tamas Csala

#include <Silice3D/common/math.hpp>

#include "cdlod/cdlod_terrain_settings.hpp"
#include "cdlod/collision/spherized_aabb.hpp"

namespace Cdlod {

namespace Math = Silice3D::Math;

SpherizedAABB::SpherizedAABB(const glm::dvec3& mins, const glm::dvec3& maxes,
                             CubeFace face, double face_size) {
  using namespace glm;

  double radius = CdlodTerrainSettings::kSphereRadius;
  radial_extent_ = {radius + mins.y, radius + maxes.y};

  /*
    The input coordinate system is right handed:
        (O)----(x - longite)
        /|
       / |
(y - rad) |
         |
    (z - latitude)

    But we can use any convenient coordinate system,
    as long as it is right handed too:

        (y)
         |
         |
         |
        (O)-----(x)
        /
       /
     (z)

    The verices:

         (E)-----(A)
         /|      /|
        / |     / |
      (F)-----(B) |
       | (H)---|-(D)
       | /     | /
       |/      |/
      (G)-----(C)
  */

  enum {
    A, B, C, D, E, F, G, H
  };

  glm::dvec3 m_vertices[8];
  m_vertices[A] = {maxes.x, maxes.y, mins.z};
  m_vertices[B] = {maxes.x, maxes.y, maxes.z};
  m_vertices[C] = {maxes.x, mins.y,  maxes.z};
  m_vertices[D] = {maxes.x, mins.y,  mins.z};
  m_vertices[E] = {mins.x,  maxes.y, mins.z};
  m_vertices[F] = {mins.x,  maxes.y, maxes.z};
  m_vertices[G] = {mins.x,  mins.y,  maxes.z};
  m_vertices[H] = {mins.x,  mins.y,  mins.z};

  glm::dvec3 vertices[8];
  for (int i = 0; i < 8; ++i) {
    vertices[i] = Cube2Sphere(m_vertices[i], face, face_size);
  }
  glm::dvec3 center = Cube2Sphere((mins + maxes)/2.0, face, face_size);

  double bsphere_radius = 0;
  for (const glm::dvec3& vertex : vertices) {
    bsphere_radius = std::max(bsphere_radius, glm::length(center - vertex));
  }
  bsphere_ = Silice3D::Sphere(center, bsphere_radius);

  enum {
    Front = 0, Right = 1, Back = 2, Left = 3
  };

  // normals are towards the inside of the AABB
  normals_[Front] = GetNormal(vertices, G, C, B, C);
  normals_[Right] = GetNormal(vertices, C, D, A, D);
  normals_[Back]  = GetNormal(vertices, D, H, E, H);
  normals_[Left]  = GetNormal(vertices, H, G, F, G);

  extents_[Front] =
    getExtent(normals_[Front], m_vertices[B], m_vertices[A], face, face_size);
  extents_[Right] =
    getExtent(normals_[Right], m_vertices[A], m_vertices[E], face, face_size);
  extents_[Back]  =
    getExtent(normals_[Back],  m_vertices[H], m_vertices[G], face, face_size);
  extents_[Left]  =
    getExtent(normals_[Left],  m_vertices[F], m_vertices[B], face, face_size);
}

glm::dvec3 SpherizedAABB::GetNormal(glm::dvec3 vertices[], int a, int b, int c, int d) {
  glm::dvec3 ba = vertices[a]-vertices[b];
  glm::dvec3 dc = vertices[c]-vertices[d];

  // If one of these are null vectors, we can't use this plane for separation,
  // so let the normal be null vector, and the (0, 0) intervals will intersect.
  if (length(ba) < Math::kEpsilon || length(dc) < Math::kEpsilon) {
    return glm::dvec3{0.0};
  }

  return normalize(cross(ba, dc));
}

bool SpherizedAABB::HasIntersection(const Interval& a, const Interval& b) {
  return a.min - Math::kEpsilon < b.max && b.min - Math::kEpsilon < a.max;
}

SpherizedAABB::Interval SpherizedAABB::getExtent(const glm::dvec3& normal,
                                                 const glm::dvec3& m_space_min,
                                                 const glm::dvec3& m_space_max,
                                                 CubeFace face, double face_size) {
  Interval interval;
  glm::dvec3 diff = m_space_max - m_space_min;
  for (int i = 0; i <= 4; ++i) {
    glm::dvec3 current = Cube2Sphere(m_space_min + i/4.0*diff, face, face_size);
    double current_projection = dot(current, normal);
    if (i == 0) {
      interval.min = current_projection;
      interval.max = current_projection;
    } else {
      interval.min = std::min(interval.min, current_projection);
      interval.max = std::max(interval.max, current_projection);
    }
  }

  return interval;
}

bool SpherizedAABB::collidesWithSphere(const Silice3D::Sphere& sphere) const {
  if (!bsphere_.CollidesWithSphere(sphere)) {
    return false;
  }

  double radial_interval_center = length(sphere.center());
  Interval radial_extent = {radial_interval_center - sphere.radius(),
                            radial_interval_center + sphere.radius()};
  if (!HasIntersection(radial_extent_, radial_extent)) {
    return false;
  }

  for (size_t i = 0; i < 4; ++i) {
    double interval_center = glm::dot(sphere.center(), normals_[i]);
    Interval projection_extent = {interval_center - sphere.radius(),
                                  interval_center + sphere.radius()};

    if (!HasIntersection(extents_[i], projection_extent)) {
      return false;
    }
  }

  return true;
}

bool SpherizedAABB::collidesWithFrustum(const Silice3D::Frustum& frustum) const {
  return bsphere_.CollidesWithFrustum(frustum);
}

SpherizedAABBDivided::SpherizedAABBDivided(const glm::dvec3& mins, const glm::dvec3& maxes,
                                           CubeFace face, double face_size)
    : main_(mins, maxes, face, face_size) {
  glm::dvec3 sub_extent = (maxes - mins) / static_cast<double>(kAabbSubdivisionRate);
  for (int x = 0; x < kAabbSubdivisionRate; ++x) {
    for (int y = 0; y < kAabbSubdivisionRate; ++y) {
      for (int z = 0; z < kAabbSubdivisionRate; ++z) {
        glm::dvec3 sub_min = mins + glm::dvec3{x, y, z} * sub_extent;
        glm::dvec3 sub_max = sub_min + sub_extent;
        subs_[Math::Sqr(kAabbSubdivisionRate)*x + kAabbSubdivisionRate*y + z]
            = SpherizedAABB{sub_min, sub_max, face, face_size};
      }
    }
  }
}

bool SpherizedAABBDivided::collidesWithSphere(const Silice3D::Sphere& sphere) const {
  if (!main_.collidesWithSphere(sphere)) {
    return false;
  }

  for (const SpherizedAABB& sub : subs_) {
    if (sub.collidesWithSphere(sphere)) {
      return true;
    }
  }

  return false;
}

bool SpherizedAABBDivided::collidesWithFrustum(const Silice3D::Frustum& frustum) const {
  if (!main_.collidesWithFrustum(frustum)) {
    return false;
  }

  for (const SpherizedAABB& sub : subs_) {
    if (sub.collidesWithFrustum(frustum)) {
      return true;
    }
  }

  return false;
}

} // namespace Cdlod

