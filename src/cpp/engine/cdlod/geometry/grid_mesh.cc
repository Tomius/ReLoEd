// Copyright (c) 2015, Tamas Csala

#include "grid_mesh.h"
#include "../../settings.h"
#include "../../oglwrap_all.h"

#define gl(func) OGLWRAP_CHECKED_FUNCTION(func)

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

void GridMesh::setupMinMax(gl::VertexAttrib attrib) {
  gl::Bind(vao_);
  gl::Bind(aMinMax_);
  attrib.setup<glm::vec2>().enable();
  attrib.divisor(1);
  gl::Unbind(vao_);
}

void GridMesh::setupTextureIds(gl::VertexAttrib attrib, int offset) {
  gl::Bind(vao_);
  gl::Bind(aTextureIds_);
  attrib.ipointer(2, gl::kUnsignedInt, 4*sizeof(glm::uvec2),
                  reinterpret_cast<const char *>(offset*sizeof(glm::uvec2)));
  attrib.divisor(1);
  attrib.enable();
  gl::Unbind(vao_);
}

void GridMesh::setupTexturePosAndSize(gl::VertexAttrib attrib, int offset) {
  gl::Bind(vao_);
  gl::Bind(aTexturePosAndSize_);
  attrib.pointer(3, gl::kFloat, false, 4*sizeof(glm::vec3),
                 reinterpret_cast<const char *>(offset*sizeof(glm::vec3)));
  attrib.divisor(1);
  attrib.enable();
  gl::Unbind(vao_);
}

void GridMesh::setupCurrentGeometryTextureIds(gl::VertexAttrib attrib) {
  setupTextureIds(attrib, 0);
}
void GridMesh::setupCurrentGeometryTexturePosAndSize(gl::VertexAttrib attrib) {
  setupTexturePosAndSize(attrib, 0);
}

void GridMesh::setupNextGeometryTextureIds(gl::VertexAttrib attrib) {
  setupTextureIds(attrib, 1);
}
void GridMesh::setupNextGeometryTexturePosAndSize(gl::VertexAttrib attrib) {
  setupTexturePosAndSize(attrib, 1);
}

void GridMesh::setupCurrentNormalTextureIds(gl::VertexAttrib attrib) {
  setupTextureIds(attrib, 2);
}
void GridMesh::setupCurrentNormalTexturePosAndSize(gl::VertexAttrib attrib) {
  setupTexturePosAndSize(attrib, 2);
}

void GridMesh::setupNextNormalTextureIds(gl::VertexAttrib attrib) {
  setupTextureIds(attrib, 3);
}
void GridMesh::setupNextNormalTexturePosAndSize(gl::VertexAttrib attrib) {
  setupTexturePosAndSize(attrib, 3);
}

void GridMesh::addToRenderList(const glm::vec4& render_data,
                               const glm::vec2& minmax,
                               const StreamedTextureInfo& texinfo) {
  render_data_.push_back(render_data);
  minmax_.push_back(minmax);

  texture_ids_.push_back(texinfo.geometry_current->id);
  texture_ids_.push_back(texinfo.geometry_next->id);
  texture_ids_.push_back(texinfo.normal_current->id);
  texture_ids_.push_back(texinfo.normal_next->id);

  texture_pos_and_size_.push_back(glm::vec3{texinfo.geometry_current->position,
                                   texinfo.geometry_current->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.geometry_next->position,
                                   texinfo.geometry_next->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.normal_current->position,
                                   texinfo.normal_current->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.normal_next->position,
                                   texinfo.normal_next->size});

  engine::Settings::geom_nodes_count++;
}

void GridMesh::clearRenderList() {
  render_data_.clear();
  minmax_.clear();
  texture_ids_.clear();
  texture_pos_and_size_.clear();
}

void GridMesh::render() {
  using gl::PrimType;
  using gl::IndexType;

  gl::Bind(vao_);
  gl::Bind(aRenderData_);
  aRenderData_.data(render_data_);

  gl::Bind(aMinMax_);
  aMinMax_.data(minmax_);

  gl::Bind(aTextureIds_);
  aTextureIds_.data(texture_ids_);

  gl::Bind(aTexturePosAndSize_);
  aTexturePosAndSize_.data(texture_pos_and_size_);

  gl::PrimitiveRestartIndex(kPrimitiveRestart);
  gl::TemporaryEnable prim_restart(gl::kPrimitiveRestart);

  if (Settings::kWireFrame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  gl::DrawElementsInstanced(PrimType::kTriangleStrip,
                            index_count_,
                            IndexType::kUnsignedShort,
                            render_data_.size());   // instance count
  if (Settings::kWireFrame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  gl::Unbind(vao_);
}

} // namespace engine

#undef gl

