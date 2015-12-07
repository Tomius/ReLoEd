// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_INFO_H_
#define ENGINE_CDLOD_TEXTURE_INFO_H_

#include <mutex>
#include <limits>
#include "../oglwrap_all.h"
#include "../settings.h"

namespace engine {

struct TextureInfo {
  uint64_t id = 0;
  glm::dvec2 position; // top-left
  double size = 0;
  size_t width = 0, height = 0;
  GLushort min = std::numeric_limits<GLushort>::max();
  GLushort max = std::numeric_limits<GLushort>::min();
  double min_h = 0;
  double max_h = Settings::kMaxHeight;

  gl::Texture2D handle;
  bool is_loaded_to_gpu = false;

  std::mutex load_mutex;
  std::vector<GLushort> data;
  bool is_loaded_to_memory = false;
};

struct StreamedTextureInfo {
  TextureInfo* geometry_current = nullptr;
  TextureInfo* geometry_next = nullptr;
  TextureInfo* normal_current = nullptr;
  TextureInfo* normal_next = nullptr;
};

}

#endif
