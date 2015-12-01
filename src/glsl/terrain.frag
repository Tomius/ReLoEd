// Copyright (c) 2015, Tamas Csala

#version 330

layout (location = 0) out vec4 fragColor;
layout (location = 1) out float fragDepth;

in VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos, m_pos;
  vec2  texCoord;
  float level, morph;
  vec4 render_data;
} vIn;

void main() {
  fragColor = vec4(0.5*dot(normalize(vIn.w_normal), vec3(0, 1, 0)) + 0.5);
  fragDepth = 0;
}
