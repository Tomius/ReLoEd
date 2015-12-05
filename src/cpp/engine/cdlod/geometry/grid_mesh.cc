// Copyright (c) 2015, Tamas Csala

#include "grid_mesh.h"
#include "../../global_height_map.h"
#include "../../oglwrap_all.h"

#define gl(func) OGLWRAP_CHECKED_FUNCTION(func)
#define UNSIGNED_INT64_ARB 0x140F

namespace engine {

GridMesh::GridMesh(GLubyte dimension) : dimension_(dimension) { }

GLushort GridMesh::indexOf(int x, int y) {
  x += dimension_/2;
  y += dimension_/2;
  return (dimension_ + 1) * y + x;
}

void GridMesh::setupPositions(gl::VertexAttrib attrib) {
  std::vector<svec2> positions;
  positions.reserve((dimension_+1) * (dimension_+1));

  GLubyte dim2 = dimension_/2;

  for (int y = -dim2; y <= dim2; ++y) {
    for (int x = -dim2; x <= dim2; ++x) {
      positions.push_back(svec2(x, y));
    }
  }

  std::vector<GLushort> indices;
  index_count_ = 2*(dimension_+1)*(dimension_) + 2*dimension_;
  indices.reserve(index_count_);

  for (int y = -dim2; y < dim2; ++y) {
    for (int x = -dim2; x <= dim2; ++x) {
      indices.push_back(indexOf(x, y));
      indices.push_back(indexOf(x, y+1));
    }
    indices.push_back (kPrimitiveRestart);
  }

  gl::Bind(vao_);
  gl::Bind(aPositions_);
  aPositions_.data(positions);
  attrib.pointer(2, gl::DataType::kShort).enable();
  gl::Unbind(aPositions_);

  gl::Bind(aIndices_);
  aIndices_.data(indices);
  gl::Unbind(vao_);
}

void GridMesh::setupRenderData(gl::VertexAttrib attrib) {
  gl::Bind(vao_);
  gl::Bind(aRenderData_);
  attrib.setup<glm::vec4>().enable();
  attrib.divisor(1);
  gl::Unbind(vao_);
}

void GridMesh::setupTextureIds(gl::VertexAttrib attrib) {
  gl::Bind(vao_);
  gl::Bind(aTextureIds_);
  attrib.setup<glm::uvec2>().enable();
  // gl(VertexAttribLPointer(aTextureIds_.expose(), 1, UNSIGNED_INT64_ARB, 0, nullptr));
  attrib.enable();
  attrib.divisor(1);
  gl::Unbind(vao_);
}

void GridMesh::setupTextureInfo(gl::VertexAttrib attrib) {
  gl::Bind(vao_);
  gl::Bind(aTextureInfo_);
  attrib.setup<glm::vec3>().enable();
  attrib.divisor(1);
  gl::Unbind(vao_);
}

void GridMesh::addToRenderList(const glm::vec4& render_data, uint64_t texture,
                               const glm::vec3& texture_info) {
  render_data_.push_back(render_data);
  assert (texture != 0);
  texture_ids_.push_back(texture);
  texture_info_.push_back(texture_info);
  engine::Settings::geom_nodes_count++;
}

void GridMesh::clearRenderList() {
  render_data_.clear();
  texture_ids_.clear();
  texture_info_.clear();
}

void GridMesh::render() {
  using gl::PrimType;
  using gl::IndexType;

  gl::Bind(vao_);
  gl::Bind(aRenderData_);
  aRenderData_.data(render_data_);

  gl::Bind(aTextureIds_);
  aTextureIds_.data(texture_ids_);

  gl::Bind(aTextureInfo_);
  aTextureInfo_.data(texture_info_);

  gl::PrimitiveRestartIndex(kPrimitiveRestart);
  gl::TemporaryEnable prim_restart(gl::kPrimitiveRestart);

  if (Settings::wire_frame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  gl::DrawElementsInstanced(PrimType::kTriangleStrip,
                            index_count_,
                            IndexType::kUnsignedShort,
                            render_data_.size());   // instance count
  if (Settings::wire_frame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  gl::Unbind(vao_);
}

} // namespace engine

#undef gl

