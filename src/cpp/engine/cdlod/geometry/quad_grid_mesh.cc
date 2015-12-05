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

void QuadGridMesh::setupTextureIds(gl::VertexAttrib attrib) {
  mesh_.setupTextureIds(attrib);
}

void QuadGridMesh::setupTextureInfo(gl::VertexAttrib attrib) {
  mesh_.setupTextureInfo(attrib);
}

// Adds a subquad to the render list.
// tl = top left, br = bottom right
void QuadGridMesh::addToRenderList(float offset_x, float offset_y,
                                   float scale, float level, uint64_t texture,
                                   const glm::vec3& texture_info,
                                   bool tl, bool tr, bool bl, bool br) {
  glm::vec4 render_data(offset_x, offset_y, scale, level);
  float dim4 = scale * mesh_.dimension()/2; // our dimension / 4
  if (tl) {
    mesh_.addToRenderList(render_data + glm::vec4(-dim4, dim4, 0, 0),
                          texture, texture_info);
  }
  if (tr) {
    mesh_.addToRenderList(render_data + glm::vec4(dim4, dim4, 0, 0),
                          texture, texture_info);
  }
  if (bl) {
    mesh_.addToRenderList(render_data + glm::vec4(-dim4, -dim4, 0, 0),
                          texture, texture_info);
  }
  if (br) {
    mesh_.addToRenderList(render_data + glm::vec4(dim4, -dim4, 0, 0),
                          texture, texture_info);
  }
}

// Adds all four subquads
void QuadGridMesh::addToRenderList(float offset_x, float offset_y, float scale,
                                   float level, uint64_t texture,
                                   const glm::vec3& texture_info) {
  addToRenderList(offset_x, offset_y, scale, level,
                  texture, texture_info, true, true, true, true);
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
