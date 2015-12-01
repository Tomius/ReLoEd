// Copyright (c) 2015, Tamas Csala

#version 330

#include "engine/cdlod_terrain.vert"

in vec2 Terrain_aPosition;
in vec4 Terrain_aRenderData;

uniform float uDepthCoef;
uniform mat4 uProjectionMatrix, uCameraMatrix, uModelMatrix;

out VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos, m_pos;
  vec2  texCoord;
  float level, morph;
  vec4 render_data;
} vOut;

void main() {
  vec3 m_normal;
  vec4 temp = Terrain_modelPos(Terrain_aPosition,
                               Terrain_aRenderData, m_normal);
  vec3 m_pos = temp.xyz;
  vOut.morph = temp.w;
  vOut.m_pos = m_pos;

  vec3 w_pos = Terrain_worldPos(m_pos);
  vec3 offseted_w_pos = (uModelMatrix * vec4(w_pos, 1)).xyz;
  vOut.w_pos = offseted_w_pos;

  vec3 m_normal_offseted_pos = m_pos + m_normal;
  vOut.w_normal = Terrain_worldPos(m_normal_offseted_pos) - w_pos;

  vOut.texCoord = Terrain_texCoord(m_pos);

  vec4 c_pos = uCameraMatrix * vec4(offseted_w_pos, 1);
  vOut.c_pos = vec3(c_pos);

  vOut.level = Terrain_aRenderData.w;
  vOut.render_data = Terrain_aRenderData;
  vec4 projected = uProjectionMatrix * c_pos;
  projected.z = log2(max(1e-6, 1.0 + projected.w)) * uDepthCoef - 1.0;
  projected.z *= projected.w;
  gl_Position = projected;
}