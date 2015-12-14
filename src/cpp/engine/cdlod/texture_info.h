// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_INFO_H_
#define ENGINE_CDLOD_TEXTURE_INFO_H_

#include <mutex>
#include "../oglwrap_all.h"

namespace engine {

struct RGBPixel {
  unsigned char r, g, b;
};

struct TextureBaseInfo {
  uint64_t id = 0;
  glm::dvec2 position; // top-left
  double size = 0;

  gl::Texture2D handle;
};

struct TextureInfo {
  TextureBaseInfo elevation, diffuse;

  std::vector<unsigned short> elevation_data;
  std::vector<RGBPixel> diffuse_data;
  bool is_loaded_to_gpu = false;

  std::mutex load_mutex;
  bool is_loaded_to_memory = false;
};

struct StreamedTextureInfo {
  TextureBaseInfo* geometry_current = nullptr;
  TextureBaseInfo* geometry_next = nullptr;
  TextureBaseInfo* normal_current = nullptr;
  TextureBaseInfo* normal_next = nullptr;
  TextureBaseInfo* diffuse_current = nullptr;
  TextureBaseInfo* diffuse_next = nullptr;
};

}

#endif
