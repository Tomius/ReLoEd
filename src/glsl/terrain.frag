// Copyright (c) 2015, Tamas Csala

#version 330
#extension GL_ARB_bindless_texture : require

#include "sky.frag"
#include "engine/bicubic_sampling.glsl"
#include "engine/cube2sphere.glsl"

layout (location = 0) out vec4 fragColor;
layout (location = 1) out float fragDepth;

in VertexData {
  vec3  c_pos, w_pos, m_pos;
  float morph; // can be useful for debugging
  flat int face;
  flat uvec2 current_tex_id, next_tex_id;
  flat vec3 current_tex_pos_and_size, next_tex_pos_and_size;
} vIn;

uniform int Terrain_uMaxHeight;
uniform int Terrain_uTextureDimension;
uniform int Terrain_uTextureDimensionWBorders;
uniform float Terrain_uSmallestTextureLodDistance;

const float kMorphEnd = 0.90, kMorphStart = 0.80;

float GetHeight(vec2 pos, uvec2 tex_id, vec3 tex_pos_and_size) {
  vec2 sample = (pos - tex_pos_and_size.xy) / tex_pos_and_size.z;
  sample += 0.5 / Terrain_uTextureDimensionWBorders;
  return textureBicubic(sampler2D(tex_id), sample).r * Terrain_uMaxHeight;
}

vec3 GetNormalModelSpaceInternal(vec2 pos, uvec2 tex_id, vec3 tex_pos_and_size) {
  float diff = tex_pos_and_size.z / Terrain_uTextureDimensionWBorders;
  float py = GetHeight(vec2(pos.x, pos.y + diff), tex_id, tex_pos_and_size);
  float my = GetHeight(vec2(pos.x, pos.y - diff), tex_id, tex_pos_and_size);
  float px = GetHeight(vec2(pos.x + diff, pos.y), tex_id, tex_pos_and_size);
  float mx = GetHeight(vec2(pos.x - diff, pos.y), tex_id, tex_pos_and_size);

  return normalize(vec3(mx-px, diff, my-py));
}

vec3 GetNormalModelSpace(vec2 pos) {
  float dist = length(vIn.c_pos);
  float next_dist = vIn.next_tex_pos_and_size.z
      / Terrain_uTextureDimension * Terrain_uSmallestTextureLodDistance;
  float morph = smoothstep(kMorphStart*next_dist, kMorphEnd*next_dist, dist);
  vec3 normal0 = GetNormalModelSpaceInternal(pos, vIn.current_tex_id,
                                              vIn.current_tex_pos_and_size);
  if (morph == 0.0) {
    return normal0;
  }

  vec3 normal1 = GetNormalModelSpaceInternal(pos, vIn.next_tex_id,
                                             vIn.next_tex_pos_and_size);

  return mix(normal0, normal1, morph);
}

vec3 GetNormal(vec2 pos) {
  return Terrain_worldPos(vIn.m_pos + GetNormalModelSpace(pos), vIn.face)
       - Terrain_worldPos(vIn.m_pos, vIn.face);
}

void main() {
  float lighting = dot(GetNormal(vIn.m_pos.xz), SunPos());
  float luminance = 0.2 + 0.6*max(lighting, 0) + 0.2 * (1+lighting)/2;
  vec3 diffuse = vec3(0.0);
  if (vIn.face/2 == 0) {
    diffuse = vec3(1, 0.9, 0.9);
  } else if (vIn.face/2 == 1) {
    diffuse = vec3(0.9, 1, 0.9);
  } else if (vIn.face/2 == 2) {
    diffuse = vec3(0.9, 0.9, 1);
  }
  fragColor = vec4(luminance*diffuse, 1);
  // fragColor = vec4(luminance*diffuse.rg, 0.9*luminance*diffuse.b + 0.1*vIn.morph, 1);
  fragDepth = length(vIn.c_pos);
}
