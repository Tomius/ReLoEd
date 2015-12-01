
#include "./cube2sphere.h"

namespace engine {

static glm::dvec3 Cubify(const glm::dvec3& p) {
  return {
    p.x * sqrt(1 - sqr(p.y)/2 - sqr(p.z)/2 + sqr(p.y*p.z)/3),
    p.y * sqrt(1 - sqr(p.z)/2 - sqr(p.x)/2 + sqr(p.z*p.x)/3),
    p.z * sqrt(1 - sqr(p.x)/2 - sqr(p.y)/2 + sqr(p.x*p.y)/3)
  };
}

}

glm::dvec3 engine::Cube2Sphere(glm::dvec3 pos,
                               CubeFace face,
                               double face_size) {
  float height = pos.y; pos.y = 0;
  pos = (pos - face_size/2) / (face_size/2);
  switch (face) {
    case CubeFace::kPosX: pos = glm::vec3{pos.z, +pos.x, pos.y}; break;
    case CubeFace::kNegX: pos = glm::vec3{pos.z, -pos.x, pos.y}; break;
    case CubeFace::kPosY: pos = glm::vec3{pos.x, +pos.y, pos.z}; break;
    case CubeFace::kNegY: pos = glm::vec3{pos.x, -pos.y, pos.z}; break;
    case CubeFace::kPosZ: pos = glm::vec3{pos.y, +pos.z, pos.x}; break;
    case CubeFace::kNegZ: pos = glm::vec3{pos.y, -pos.z, pos.x}; break;
  }
  return (face_size + height) * Cubify(pos);
}
