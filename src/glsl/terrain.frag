// Copyright (c) 2015, Tamas Csala

#version 330
#extension GL_ARB_bindless_texture : require

layout (location = 0) out vec4 fragColor;
layout (location = 1) out float fragDepth;

in VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos, m_pos;
  vec2  texCoord;
  float level, morph;
  vec4 render_data;
  flat uvec2 texId;
  flat vec3 texInfo;
} vIn;

uniform int Terrain_uFace;
uniform int Terrain_uMaxHeight;

float GetHeight(vec2 pos) {
  vec2 sample = (pos - vIn.texInfo.xy) / vIn.texInfo.z;
  return texture(sampler2D(vIn.texId), sample).r;
}

vec3 GetNormal(vec2 pos) {
  float diff = vIn.texInfo.z / 262;
  float py = GetHeight(vec2(pos.x, pos.y + diff)) * Terrain_uMaxHeight;
  float my = GetHeight(vec2(pos.x, pos.y - diff)) * Terrain_uMaxHeight;
  float px = GetHeight(vec2(pos.x + diff, pos.y)) * Terrain_uMaxHeight;
  float mx = GetHeight(vec2(pos.x - diff, pos.y)) * Terrain_uMaxHeight;

  return normalize(vec3(px-mx, diff, py-my));
}

void main() {
  float height = GetHeight(vIn.m_pos.xz);
  float luminance = 0.2 + max(dot(GetNormal(vIn.m_pos.xz), normalize(vec3(1, 1, 0))), 0);
  vec3 diffuse = vec3(0.0);
  if (Terrain_uFace/2 == 0) {
    diffuse = vec3(1, 0.75, 0.75);
  } else if (Terrain_uFace/2 == 1) {
    diffuse = vec3(0.75, 1, 0.75);
  } else if (Terrain_uFace/2 == 2) {
    diffuse = vec3(0.75, 0.75, 1);
  }
  fragColor = vec4(luminance*diffuse, 1);
  fragDepth = length(vIn.c_pos);
}
