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

// Adds a subquad to the render list.
// tl = top left, br = bottom right
void QuadGridMesh::addToRenderList(float offset_x, float offset_y,
                                   float scale, float level,
                                   bool tl, bool tr, bool bl, bool br) {
  glm::vec4 render_data(offset_x, offset_y, scale, level);
  float dim4 = scale * mesh_.dimension()/2; // our dimension / 4
  if(tl) { mesh_.addToRenderList(render_data + glm::vec4(-dim4, dim4, 0, 0)); }
  if(tr) { mesh_.addToRenderList(render_data + glm::vec4(dim4, dim4, 0, 0)); }
  if(bl) { mesh_.addToRenderList(render_data + glm::vec4(-dim4, -dim4, 0, 0)); }
  if(br) { mesh_.addToRenderList(render_data + glm::vec4(dim4, -dim4, 0, 0)); }
}

// Adds all four subquads
void QuadGridMesh::addToRenderList(float offset_x, float offset_y,
                                   float scale, float level) {
  addToRenderList(offset_x, offset_y, scale, level, true, true, true, true);
  engine::GlobalHeightMap::geom_nodes_count++;
}

void QuadGridMesh::clearRenderList() {
  mesh_.clearRenderList();
  engine::GlobalHeightMap::geom_nodes_count = 0;
}

void QuadGridMesh::render() {
  mesh_.render();
}

size_t QuadGridMesh::node_count() const {
  return mesh_.node_count();
}

} // namespace engine
