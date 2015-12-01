// Copyright (c) 2015, Tamas Csala

#include "./bounding_box.h"

namespace engine {

bool BoundingBox::collidesWithSphere(const Sphere& sphere) const {
  double dmin = 0;
  for (int i = 0; i < 3; ++i) {
    if (sphere.center()[i] < mins_[i]) {
      dmin += sqr(sphere.center()[i] - mins_[i]);
    } else if (sphere.center()[i] > maxes_[i]) {
      dmin += sqr(sphere.center()[i] - maxes_[i]);
    }
  }
  return dmin <= sqr(sphere.radius());
}

bool BoundingBox::collidesWithFrustum(const Frustum& frustum) const {
  glm::dvec3 center = this->center();
  glm::dvec3 extent = this->extent();

  for(int i = 0; i < 6; ++i) {
    const Plane& plane = frustum.planes[i];

    double d = glm::dot(center, plane.normal);
    double r = glm::dot(extent, glm::abs(plane.normal));

    if(d + r < -plane.dist) {
      return false;
    }
  }
  return true;
}

}
