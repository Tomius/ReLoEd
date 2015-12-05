// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_GRID_MESH_H_
#define ENGINE_CDLOD_QUAD_GRID_MESH_H_

#include "grid_mesh.h"
#include "../../global_height_map.h"

namespace engine {

// Makes up four, separately renderable GridMeshes.
class QuadGridMesh {
  GridMesh mesh_;

 public:
  // Specify the size of the 4 subquads together, not the size of one subquad
  // It should be between 2 and 256, and should be a power of 2
  QuadGridMesh(int dimension = Settings::node_dimension);

  void setupPositions(gl::VertexAttrib attrib);
  void setupRenderData(gl::VertexAttrib attrib);
  void setupTextureIds(gl::VertexAttrib attrib);
  void setupTextureInfo(gl::VertexAttrib attrib);

  // Adds a subquad to the render list. tl = top left, br = bottom right
  void addToRenderList(float offset_x, float offset_y, float scale, float level,
                       uint64_t texture, const glm::vec3& texture_info,
                       bool tl, bool tr, bool bl, bool br);
  // Adds all four subquads
  void addToRenderList(float offset_x, float offset_y, float scale,
                       float level, uint64_t texture,
                       const glm::vec3& texture_info);
  void clearRenderList();
  void render();
  size_t node_count() const;
};

} // namespace engine

#endif
