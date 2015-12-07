#include "./shader_manager.h"
#include "./game_engine.h"

namespace engine {

static gl::ShaderType get_shader_type(std::string& filename,
                                      const ShaderFile* included_from) {
  size_t dot_position = filename.find_last_of('.');
  if (dot_position != std::string::npos) {
    std::string extension = filename.substr(dot_position+1);
    if (extension == "frag") {
      return gl::kFragmentShader;
    } else if (extension == "vert") {
      return gl::kVertexShader;
    } else if (extension == "geom") {
      return gl::kGeometryShader;
    } else if (extension == "glsl" && included_from != nullptr) {
      gl::ShaderType type = included_from->shader_type();
      if (type == gl::kFragmentShader) {
        filename.replace(dot_position+1, 4, "frag");
      } else if (type == gl::kVertexShader) {
        filename.replace(dot_position+1, 4, "vert");
      } else {
        filename.replace(dot_position+1, 4, "geom");
      }
      return type;
    } else{
      throw std::invalid_argument("Can't guess the shader type of " + filename);
    }
  } else {
    throw std::invalid_argument("Can't guess the shader type of " + filename);
  }
}

ShaderFile* ShaderManager::publish(const std::string& filename,
                                   const gl::ShaderSource& src) {
  return load(filename, src);
}

ShaderFile* ShaderManager::get(const std::string& filename,
                               const ShaderFile* included_from) {
  auto iter = shaders_.find(filename);
  if (iter != shaders_.end()) {
    return iter->second.get();
  } else {
    ShaderFile* shader = load(filename, included_from);
    return shader;
  }
}

ShaderFile::ShaderFile(std::string filename, const gl::ShaderSource& src,
                       const ShaderFile* included_from)
    : gl::Shader(get_shader_type(filename, included_from)) {
  std::string src_str = src.source();
  findIncludes(src_str);
  for (ShaderFile *included : includes_) {
    if (included->state_ == gl::Shader::kCompileFailure) {
      state_ = gl::Shader::kCompileFailure;
      return;
    }
  }
  findExports(src_str);
  set_source(src_str);
  set_source_file(filename);
  compile();
}

void ShaderFile::findExports(std::string &src) {
  // search for the exported functions
  size_t export_pos = src.find("#export");
  while (export_pos != std::string::npos) {
    size_t line_end = src.find("\n", export_pos);
    size_t start_pos =
        src.find_first_not_of(" \t", export_pos + sizeof("#export") - 1);

    // Add the exported entity to exports_. The exports musn't be separated
    // with newline characters, it would mess up the line numbers, and the
    // GLSL error messages would be a lot harder to understand.
    exports_ += src.substr(start_pos, line_end - start_pos);

    // remove the export macro (but leave the declaration and the \n)
    src.erase(export_pos, start_pos - export_pos);

    // Search for the next #export
    export_pos = src.find("#export");
  }
}

void ShaderFile::findIncludes(std::string &src) {
  // look for #includes, search for the ShadingTechnique to add, then
  // call addToProgram on it. After that replace the include line
  // with the declarations of the functions, the tech provides.
  size_t include_pos = src.find("#include");
  while (include_pos != std::string::npos) {
    size_t line_end = src.find("\n", include_pos);
    size_t start_comma = src.find("\"", include_pos);
    size_t end_comma = src.find("\"", start_comma+1);

    std::string included_filename =
        src.substr(start_comma+1, end_comma-start_comma-1);

    ShaderFile *included_shader =
      GameEngine::shader_manager()->get(included_filename, this);
    includes_.push_back(included_shader);

    // Replace the include directive with the included statements
    src.replace(include_pos, line_end - include_pos,
                included_shader->exports());

    // Search for the next #include
    include_pos = src.find("#include");
  }
}

}  // namespace engine
