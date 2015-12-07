// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_SHADER_MANAGER_H_
#define ENGINE_SHADER_MANAGER_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "./oglwrap_all.h"

namespace engine {

class ShaderFile;
class ShaderProgram;
class ShaderManager {
  std::map<std::string, std::unique_ptr<ShaderFile>> shaders_;
  template<typename... Args>
  ShaderFile* load(Args&&... args);
 public:
  ShaderFile* publish(const std::string& filename, const gl::ShaderSource& src);
  ShaderFile* get(const std::string& filename,
                  const ShaderFile* included_from = nullptr);
};

class ShaderFile : public gl::Shader {
 public:
  ShaderFile(std::string filename, const ShaderFile* included_from = nullptr)
      : ShaderFile(filename, gl::ShaderSource{filename}, included_from) {}

  ShaderFile(std::string filename, const gl::ShaderSource& src,
             const ShaderFile* included_from = nullptr);

  void set_update_func(std::function<void(const gl::Program&)> func) {
    update_func_ = func;
  }

  void update(const gl::Program& prog) const {
    if (update_func_) {
      update_func_(prog);
    }
  }

  const std::string& exports() const { return exports_; }

 private:
  std::function<void(const gl::Program&)> update_func_;
  std::vector<ShaderFile*> includes_;
  std::string exports_;

  void findExports(std::string &src);
  void findIncludes(std::string &src);

  friend class ShaderProgram;
};

class ShaderProgram : public gl::Program {
 public:
  ShaderProgram() {}

  template <typename... Shaders>
  explicit ShaderProgram(ShaderFile *shader, Shaders&&... shaders) {
    attachShaders(shader, shaders...);
    link();
  }

  ShaderProgram(const ShaderProgram& prog) = default;
  ShaderProgram(ShaderProgram&& prog) = default;

  void update() const {
    for (auto shader : shaders_) {
      shader->update(*this);
    }
  }

  template<typename... Rest>
  ShaderProgram& attachShaders(ShaderFile *shader, Rest&&... rest) {
    attachShader(shader);
    attachShaders(rest...);
    return *this;
  }
  ShaderProgram& attachShaders() {
    return *this;
  }

  // Depth First Search for all the included files, recursively
  ShaderProgram& attachShader(ShaderFile *shader) {
    if (shaders_.insert(shader).second) {
      for (auto include : shader->includes_) {
        attachShader(include);
      }
    }
    return *this;
  }

  virtual const Program& link() override {
    for (auto shader_file : shaders_) {
      const gl::Shader& shader = *shader_file;
      gl::Program::attachShader(shader);
    }
    gl::Program::link();

    return *this;
  }

 private:
  std::set<ShaderFile*> shaders_;
};

}  // namespace engine

#include "./shader_manager-inl.h"

#endif
