#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

static const std::string output_dir = "/media/icecool/Data/BME/szakdoga/height";
constexpr int kInputWidth = 172800, kInputHeight = 86400;
constexpr long kFaceSize = 65536;
constexpr long kRadius = kFaceSize / 2;
constexpr int kBorderSize = 3;
constexpr int kTexNodeDimension = 256;
constexpr int kMaxLevel = 8;
constexpr int kImageCount = 21845; //87380; // (4^9-1)/(4-1)/4

template <typename T, glm::precision P>
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec2<T, P>& v) {
  os << v.x << ", " << v.y;
  return os;
}

template <typename T, glm::precision P>
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec3<T, P>& v) {
  os << v.x << ", " << v.y << ", " << v.z;
  return os;
}

template <typename T, glm::precision P>
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec4<T, P>& v) {
  os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
  return os;
}

template<typename T>
auto sqr(const T& x) {
  return x * x;
}


inline double clamp(double x, double min, double max) {
  if (x < min) return min;
  else if (x > max) return max;
  else return x;
}
