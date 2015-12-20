#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

template <class T>
inline constexpr T pow(T const& x, std::size_t n){
    return n>0 ? x * pow(x, n-1) : 1;
}

#define HEIGHTMAP

#ifdef HEIGHTMAP
  static const std::string kInputDir = "/media/icecool/Data/LoE_datasets/height/gmted2010_75/uv";
  static const std::string kOutputDir = "/media/icecool/SSData/gmted2010_75_cube";
  // static const std::string kOutputDir = "output";

  constexpr int kInputWidth = 172800, kInputHeight = 86400;
  constexpr int kInputMaxLevel = 9;
  constexpr int kBorderSize = 3;

  constexpr long kFaceSize = 65536;
  constexpr int kMaxLevel = 8;
#else
  static const std::string kInputDir = "/media/icecool/Data/LoE_datasets/diffuse/blue_marble_next_gen/uv";
  static const std::string kOutputDir = "/home/icecool/projects/C++/OpenGL/ReLoEd/src/resources/textures/diffuse";
  // static const std::string kOutputDir = "output";

  constexpr int kInputWidth = 86400, kInputHeight = 43200;
  constexpr int kInputMaxLevel = 8;
  constexpr int kBorderSize = 2;

  constexpr long kFaceSize = 32768;
  constexpr int kMaxLevel = 7;
#endif

constexpr long kRadius = kFaceSize / 2;
constexpr int kTexNodeDimension = 256;
constexpr int kMinLevel = 0;
constexpr int kImageCount = pow(4, kMaxLevel-kMinLevel+1) / (4-1);

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
