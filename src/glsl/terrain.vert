// Copyright (c) 2015, Tamas Csala

#version 330

#include "engine/cdlod_terrain.vert"
vec3 Terrain_worldPos(vec3 pos, int face); // todo

in vec2 Terrain_aPosition;
in vec4 Terrain_aRenderData;

in uvec2 Terrain_aCurrentGeometryTextureId;
in vec3 Terrain_aCurrentGeometryTexturePosAndSize;

in uvec2 Terrain_aNextGeometryTextureId;
in vec3 Terrain_aNextGeometryTexturePosAndSize;

in uvec2 Terrain_aCurrentNormalTextureId;
in vec3 Terrain_aCurrentNormalTexturePosAndSize;

in uvec2 Terrain_aNextNormalTextureId;
in vec3 Terrain_aNextNormalTexturePosAndSize;

in uvec2 Terrain_aCurrentDiffuseTextureId;
in vec3 Terrain_aCurrentDiffuseTexturePosAndSize;

in uvec2 Terrain_aNextDiffuseTextureId;
in vec3 Terrain_aNextDiffuseTexturePosAndSize;

uniform float uDepthCoef;
uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;

out VertexData {
  vec3 c_pos, w_pos, m_pos;
  float morph;
  flat int face;

  flat uvec2 current_normal_tex_id, next_normal_tex_id;
  flat vec3 current_normal_tex_pos_and_size, next_normal_tex_pos_and_size;

  flat uvec2 current_diffuse_tex_id, next_diffuse_tex_id;
  flat vec3 current_diffuse_tex_pos_and_size, next_diffuse_tex_pos_and_size;
} vOut;

void main() {
  vec4 temp = Terrain_modelPos(Terrain_aPosition);
  vec3 m_pos = temp.xyz;
  vOut.morph = temp.w;
  vOut.m_pos = m_pos;

  vec3 w_pos = Terrain_worldPos(m_pos, Terrain_face());
  vec3 offseted_w_pos = (uModelMatrix * vec4(w_pos, 1)).xyz;
  vOut.w_pos = offseted_w_pos;

  vec4 c_pos = uCameraMatrix * vec4(offseted_w_pos, 1);
  vOut.c_pos = vec3(c_pos);

  vOut.face = Terrain_face();
  vOut.current_normal_tex_id = Terrain_aCurrentNormalTextureId;
  vOut.current_normal_tex_pos_and_size = Terrain_aCurrentNormalTexturePosAndSize;
  vOut.next_normal_tex_id = Terrain_aNextNormalTextureId;
  vOut.next_normal_tex_pos_and_size = Terrain_aNextNormalTexturePosAndSize;

  vOut.current_diffuse_tex_id = Terrain_aCurrentDiffuseTextureId;
  vOut.current_diffuse_tex_pos_and_size = Terrain_aCurrentDiffuseTexturePosAndSize;
  vOut.next_diffuse_tex_id = Terrain_aNextDiffuseTextureId;
  vOut.next_diffuse_tex_pos_and_size = Terrain_aNextDiffuseTexturePosAndSize;

  vec4 projected = uProjectionMatrix * c_pos;
  projected.z = log2(max(1e-6, 1.0 + projected.w)) * uDepthCoef - 1.0;
  projected.z *= projected.w;
  gl_Position = projected;
}
