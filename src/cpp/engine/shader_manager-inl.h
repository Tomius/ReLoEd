// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_SHADER_MANAGER_INL_H_
#define ENGINE_SHADER_MANAGER_INL_H_

#include <string>
#include "./shader_manager.h"

namespace engine {

template<typename... Args>
ShaderFile* ShaderManager::load(Args&&... args) {
  auto shader = new ShaderFile{std::forward<Args>(args)...};
  shaders_[shader->source_file()] = std::unique_ptr<ShaderFile>{shader};
  return shader;
}

}  // namespace engine

#endif

