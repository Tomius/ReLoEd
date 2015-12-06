// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_INFO_H_
#define ENGINE_CDLOD_TEXTURE_INFO_H_

#include <mutex>
#include "../oglwrap_all.h"

namespace engine {

struct TextureInfo {
  uint64_t id = 0;
  glm::dvec2 position; // top-left
  double size = 0;
  size_t width = 0, height = 0;

  gl::Texture2D handle;
  bool is_loaded_to_gpu = false;

  std::mutex load_mutex;
  std::vector<unsigned short> data;
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
