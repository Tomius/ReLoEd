// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_GRID_MESH_H_
#define ENGINE_CDLOD_QUAD_GRID_MESH_H_

#include "grid_mesh.h"
#include "../../settings.h"
#include "../texture_info.h"

namespace engine {

// Makes up four, separately renderable GridMeshes.
class QuadGridMesh {
  GridMesh mesh_;

 public:
  // Specify the size of the 4 subquads together, not the size of one subquad
  // It should be between 2 and 256, and should be a power of 2
  QuadGridMesh(int dimension = Settings::kNodeDimension);

  void setupPositions(gl::VertexAttrib attrib);
  void setupRenderData(gl::VertexAttrib attrib);

  void setupCurrentGeometryTextureIds(gl::VertexAttrib attrib);
  void setupCurrentGeometryTexturePosAndSize(gl::VertexAttrib attrib);
  void setupNextGeometryTextureIds(gl::VertexAttrib attrib);
  void setupNextGeometryTexturePosAndSize(gl::VertexAttrib attrib);

  void setupCurrentNormalTextureIds(gl::VertexAttrib attrib);
  void setupCurrentNormalTexturePosAndSize(gl::VertexAttrib attrib);
  void setupNextNormalTextureIds(gl::VertexAttrib attrib);
  void setupNextNormalTexturePosAndSize(gl::VertexAttrib attrib);

  void setupCurrentDiffuseTextureIds(gl::VertexAttrib attrib);
  void setupCurrentDiffuseTexturePosAndSize(gl::VertexAttrib attrib);
  void setupNextDiffuseTextureIds(gl::VertexAttrib attrib);
  void setupNextDiffuseTexturePosAndSize(gl::VertexAttrib attrib);

  // Adds a subquad to the render list. tl = top left, br = bottom right
  void addToRenderList(float offset_x, float offset_y, int level, int face,
                       const StreamedTextureInfo& texinfo,
                       bool tl, bool tr, bool bl, bool br);
  // Adds all four subquads
  void addToRenderList(float offset_x, float offset_y, int level, int face,
                       const StreamedTextureInfo& texinfo);
  void clearRenderList();
  void render();
  size_t node_count() const;
};

} // namespace engine

#endif
