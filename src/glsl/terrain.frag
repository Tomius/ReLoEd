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

float Terrain_getHeight(vec2 pos) {
  vec2 sample = (pos - vIn.texInfo.xy) / vIn.texInfo.z;
  return texture(sampler2D(vIn.texId), sample).r;
}

void main() {
  fragColor = vec4(pow(Terrain_getHeight(vIn.m_pos.xz), 1/2.2));
  // fragColor = vec4(0.5*dot(normalize(vIn.w_normal), vec3(0, 1, 0)) + 0.5);
  fragDepth = 0;
}
