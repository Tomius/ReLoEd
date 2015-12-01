// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_COLLISION_BOUNDING_BOX_H_
#define ENGINE_COLLISION_BOUNDING_BOX_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "../misc.h"
#include "./frustum.h"
#include "./sphere.h"

namespace engine {

class BoundingBox {
 protected:
  glm::dvec3 mins_;
  glm::dvec3 maxes_;
 public:
  BoundingBox() = default;

  BoundingBox(const glm::dvec3& mins, const glm::dvec3& maxes)
      : mins_(mins), maxes_(maxes) {}

  glm::dvec3 mins() const { return mins_; }
  glm::dvec3 maxes() const { return maxes_; }
  glm::dvec3 center() const { return (maxes_+mins_) / 2.0; }
  glm::dvec3 extent() const { return maxes_-mins_; }

  virtual bool collidesWithSphere(const Sphere& sphere) const;
  virtual bool collidesWithFrustum(const Frustum& frustum) const;
};

}


#endif
