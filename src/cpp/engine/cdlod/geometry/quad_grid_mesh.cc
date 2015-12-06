// Copyright (c) 2015, Tamas Csala

#include "quad_grid_mesh.h"

namespace engine {

QuadGridMesh::QuadGridMesh(int dimension) : mesh_(dimension/2) {
  assert(2 <= dimension && dimension <= 256);
}

void QuadGridMesh::setupPositions(gl::VertexAttrib attrib) {
  mesh_.setupPositions(attrib);
}

void QuadGridMesh::setupRenderData(gl::VertexAttrib attrib) {
  mesh_.setupRenderData(attrib);
}

void QuadGridMesh::setupCurrentGeometryTextureIds(gl::VertexAttrib attrib) {
  mesh_.setupCurrentGeometryTextureIds(attrib);
}
void QuadGridMesh::setupCurrentGeometryTexturePosAndSize(gl::VertexAttrib attrib) {
  mesh_.setupCurrentGeometryTexturePosAndSize(attrib);
}
void QuadGridMesh::setupNextGeometryTextureIds(gl::VertexAttrib attrib) {
  mesh_.setupNextGeometryTextureIds(attrib);
}
void QuadGridMesh::setupNextGeometryTexturePosAndSize(gl::VertexAttrib attrib) {
  mesh_.setupNextGeometryTexturePosAndSize(attrib);
}

void QuadGridMesh::setupCurrentNormalTextureIds(gl::VertexAttrib attrib) {
  mesh_.setupCurrentNormalTextureIds(attrib);
}
void QuadGridMesh::setupCurrentNormalTexturePosAndSize(gl::VertexAttrib attrib) {
  mesh_.setupCurrentNormalTexturePosAndSize(attrib);
}
void QuadGridMesh::setupNextNormalTextureIds(gl::VertexAttrib attrib) {
  mesh_.setupNextNormalTextureIds(attrib);
}
void QuadGridMesh::setupNextNormalTexturePosAndSize(gl::VertexAttrib attrib) {
  mesh_.setupNextNormalTexturePosAndSize(attrib);
}

// Adds a subquad to the render list.
// tl = top left, br = bottom right
void QuadGridMesh::addToRenderList(float offset_x, float offset_y,
                                   float scale, float level,
                                   const StreamedTextureInfo& texinfo,
                                   bool tl, bool tr, bool bl, bool br) {
  glm::vec4 render_data(offset_x, offset_y, scale, level);
  float dim4 = scale * mesh_.dimension()/2; // our dimension / 4
  if (tl) {
    mesh_.addToRenderList(render_data + glm::vec4(-dim4, dim4, 0, 0), texinfo);
  }
  if (tr) {
    mesh_.addToRenderList(render_data + glm::vec4(dim4, dim4, 0, 0), texinfo);
  }
  if (bl) {
    mesh_.addToRenderList(render_data + glm::vec4(-dim4, -dim4, 0, 0), texinfo);
  }
  if (br) {
    mesh_.addToRenderList(render_data + glm::vec4(dim4, -dim4, 0, 0), texinfo);
  }
}

// Adds all four subquads
void QuadGridMesh::addToRenderList(float offset_x, float offset_y,
                                   float scale, float level,
                                   const StreamedTextureInfo& texinfo) {
  addToRenderList(offset_x, offset_y, scale, level,
                  texinfo, true, true, true, true);
}

void QuadGridMesh::clearRenderList() {
  mesh_.clearRenderList();
}

void QuadGridMesh::render() {
  mesh_.render();
}

size_t QuadGridMesh::node_count() const {
  return mesh_.node_count();
}

} // namespace engine
