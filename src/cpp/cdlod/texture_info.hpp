// Copyright (c), Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_INFO_H_
#define ENGINE_CDLOD_TEXTURE_INFO_H_

#include <mutex>
#include <limits>
#include <glad/glad.h>
#include <oglwrap/oglwrap.h>

#include "cdlod/cdlod_terrain_settings.hpp"

namespace Cdlod {

struct RGBPixel {
  unsigned char r = 0, g = 0, b = 0;
};

struct TextureBaseInfo {
  uint64_t id = 0;
  glm::dvec2 position {0.0, 0.0}; // top-left
  double size = 0;

  gl::Texture2D handle;
};

class CdlodQuadTreeNode;

struct TextureInfo {
  TextureBaseInfo elevation, diffuse;

  GLushort min = std::numeric_limits<GLushort>::max();
  GLushort max = std::numeric_limits<GLushort>::min();
  double min_h = 0;
  double max_h = CdlodTerrainSettings::kMaxHeight;

  CdlodQuadTreeNode* min_max_src = nullptr;

  std::vector<GLushort> elevation_data;
  std::vector<RGBPixel> diffuse_data;
  bool is_loaded_to_gpu = false;

  std::mutex load_mutex;
  bool is_loaded_to_memory = false;

  TextureInfo() = default;
  TextureInfo(TextureInfo&& other)
    : elevation(std::move(other.elevation))
    , diffuse(std::move(other.diffuse))
    , min(other.min)
    , max(other.max)
    , min_h(other.min_h)
    , max_h(other.max_h)
    , min_max_src(other.min_max_src)
    , elevation_data(std::move(other.elevation_data))
    , diffuse_data(std::move(other.diffuse_data))
    , is_loaded_to_gpu(other.is_loaded_to_gpu)
    , is_loaded_to_memory (other.is_loaded_to_memory)
  {}
};

struct StreamedTextureInfo {
  TextureBaseInfo* geometry_current = nullptr;
  TextureBaseInfo* geometry_next = nullptr;
  TextureBaseInfo* normal_current = nullptr;
  TextureBaseInfo* normal_next = nullptr;
  TextureBaseInfo* diffuse_current = nullptr;
  TextureBaseInfo* diffuse_next = nullptr;
};

} // namespace Cdlod

#endif
