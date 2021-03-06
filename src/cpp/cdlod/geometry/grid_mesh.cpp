// Copyright (c), Tamas Csala

#include "cdlod/geometry/grid_mesh.hpp"
#include "cdlod/cdlod_terrain_settings.hpp"

namespace Cdlod {

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

void GridMesh::setupTextureIds(gl::VertexAttrib attrib, int offset) {
  gl::Bind(vao_);
  gl::Bind(aTextureIds_);
  attrib.ipointer(2, gl::kUnsignedInt, 6*sizeof(glm::uvec2),
                  reinterpret_cast<const char *>(offset*sizeof(glm::uvec2)));
  attrib.divisor(1);
  attrib.enable();
  gl::Unbind(vao_);
}

void GridMesh::setupTexturePosAndSize(gl::VertexAttrib attrib, int offset) {
  gl::Bind(vao_);
  gl::Bind(aTexturePosAndSize_);
  attrib.pointer(3, gl::kFloat, false, 6*sizeof(glm::vec3),
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

void GridMesh::setupCurrentDiffuseTextureIds(gl::VertexAttrib attrib) {
  setupTextureIds(attrib, 4);
}
void GridMesh::setupCurrentDiffuseTexturePosAndSize(gl::VertexAttrib attrib) {
  setupTexturePosAndSize(attrib, 4);
}

void GridMesh::setupNextDiffuseTextureIds(gl::VertexAttrib attrib) {
  setupTextureIds(attrib, 5);
}
void GridMesh::setupNextDiffuseTexturePosAndSize(gl::VertexAttrib attrib) {
  setupTexturePosAndSize(attrib, 5);
}

void GridMesh::addToRenderList(const glm::vec4& render_data,
                               const StreamedTextureInfo& texinfo) {
  render_data_.push_back(render_data);

  texture_ids_.push_back(texinfo.geometry_current->handle.bindless_handle());
  texture_ids_.push_back(texinfo.geometry_next->handle.bindless_handle());
  texture_ids_.push_back(texinfo.normal_current->handle.bindless_handle());
  texture_ids_.push_back(texinfo.normal_next->handle.bindless_handle());
  texture_ids_.push_back(texinfo.diffuse_current->handle.bindless_handle());
  texture_ids_.push_back(texinfo.diffuse_next->handle.bindless_handle());

  texture_pos_and_size_.push_back(glm::vec3{texinfo.geometry_current->position,
                                   texinfo.geometry_current->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.geometry_next->position,
                                   texinfo.geometry_next->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.normal_current->position,
                                   texinfo.normal_current->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.normal_next->position,
                                   texinfo.normal_next->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.diffuse_current->position,
                                   texinfo.diffuse_current->size});
  texture_pos_and_size_.push_back(glm::vec3{texinfo.diffuse_next->position,
                                   texinfo.diffuse_next->size});

  CdlodTerrainSettings::geom_nodes_count++;
}

void GridMesh::clearRenderList() {
  render_data_.clear();
  texture_ids_.clear();
  texture_pos_and_size_.clear();
}

void GridMesh::render() {
  using gl::PrimType;
  using gl::IndexType;

  gl::Bind(vao_);
  gl::Bind(aRenderData_);
  aRenderData_.data(render_data_);

  gl::Bind(aTextureIds_);
  aTextureIds_.data(texture_ids_);

  gl::Bind(aTexturePosAndSize_);
  aTexturePosAndSize_.data(texture_pos_and_size_);

  gl::PrimitiveRestartIndex(kPrimitiveRestart);
  gl::TemporaryEnable prim_restart(gl::kPrimitiveRestart);

  if (CdlodTerrainSettings::kWireFrame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  gl::DrawElementsInstanced(PrimType::kTriangleStrip,
                            index_count_,
                            IndexType::kUnsignedShort,
                            render_data_.size());   // instance count
  if (CdlodTerrainSettings::kWireFrame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  gl::Unbind(vao_);
}

} // namespace Cdlod


