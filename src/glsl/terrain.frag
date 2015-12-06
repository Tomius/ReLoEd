// Copyright (c) 2015, Tamas Csala

#version 330
#extension GL_ARB_bindless_texture : require

#include "sky.frag"

layout (location = 0) out vec4 fragColor;
layout (location = 1) out float fragDepth;

in VertexData {
  vec3  w_normal;
  vec3  c_pos, w_pos, m_pos;
  vec2  texCoord;
  float level, morph;
  vec4 render_data;
  flat uvec2 current_tex_id, next_tex_id;
  flat vec3 current_tex_pos_and_size, next_tex_pos_and_size;
} vIn;

uniform int Terrain_uFace;
uniform int Terrain_uMaxHeight;
uniform int Terrain_uTextureDimension;
uniform int Terrain_uTextureDimensionWBorders;
uniform ivec2 Terrain_uTexSize;
uniform float Terrain_uSmallestTextureLodDistance;
float Terrain_radius = Terrain_uTexSize.x / 2;

const float kMorphEnd = 0.95, kMorphStart = 0.85;

vec4 cubic(float v){
  vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
  vec4 s = n * n * n;
  float x = s.x;
  float y = s.y - 4.0 * s.x;
  float z = s.z - 4.0 * s.y + 6.0 * s.x;
  float w = 6.0 - x - y - z;
  return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 textureBicubic(sampler2D tex, vec2 texCoords){
  vec2 texSize = textureSize(tex, 0);
  vec2 invTexSize = 1.0 / texSize;

  texCoords = texCoords * texSize - 0.5;
  vec2 fxy = fract(texCoords);
  texCoords -= fxy;

  vec4 xcubic = cubic(fxy.x);
  vec4 ycubic = cubic(fxy.y);

  vec4 c = texCoords.xxyy + vec2(-0.5, +1.5).xyxy;

  vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
  vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;

  offset *= invTexSize.xxyy;

  vec4 sample0 = texture(tex, offset.xz);
  vec4 sample1 = texture(tex, offset.yz);
  vec4 sample2 = texture(tex, offset.xw);
  vec4 sample3 = texture(tex, offset.yw);

  float sx = s.x / (s.x + s.y);
  float sy = s.z / (s.z + s.w);

  return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

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

  return normalize(vec3(px-mx, diff, py-my));
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


#define kPosX 0
#define kNegX 1
#define kPosY 2
#define kNegY 3
#define kPosZ 4
#define kNegZ 5

float sqr(float x) {
  return x * x;
}

vec3 Cubify(vec3 p) {
  return vec3(
    -p.x * sqrt(1 - sqr(p.y)/2 - sqr(p.z)/2 + sqr(p.y*p.z)/3),
    -p.y * sqrt(1 - sqr(p.z)/2 - sqr(p.x)/2 + sqr(p.z*p.x)/3),
    p.z * sqrt(1 - sqr(p.x)/2 - sqr(p.y)/2 + sqr(p.x*p.y)/3)
  );
}

vec3 Terrain_worldPos(vec3 pos) {
  float height = pos.y; pos.y = 0;
  pos = (pos - Terrain_uTexSize.x/2) / (Terrain_uTexSize.x/2);
  switch (Terrain_uFace) {
    case kPosX: pos = vec3(+pos.y, +pos.z, -pos.x); break;
    case kNegX: pos = vec3(-pos.y, +pos.z, +pos.x); break;
    case kPosY: pos = vec3(+pos.z, +pos.y, +pos.x); break;
    case kNegY: pos = vec3(-pos.z, -pos.y, +pos.x); break;
    case kPosZ: pos = vec3(+pos.x, +pos.z, +pos.y); break;
    case kNegZ: pos = vec3(-pos.x, +pos.z, -pos.y); break;
  }
  return (Terrain_radius + height) * Cubify(pos);
}

vec3 GetNormal(vec2 pos) {
  return Terrain_worldPos(vIn.m_pos + GetNormalModelSpace(pos))
       - Terrain_worldPos(vIn.m_pos);
}

void main() {
  float lighting = dot(GetNormal(vIn.m_pos.xz), SunPos());
  float luminance = 0.2 + 0.6*max(lighting, 0) + 0.2 * (1+lighting)/2;
  vec3 diffuse = vec3(0.0);
  if (Terrain_uFace/2 == 0) {
    diffuse = vec3(1, 0.9, 0.9);
  } else if (Terrain_uFace/2 == 1) {
    diffuse = vec3(0.9, 1, 0.9);
  } else if (Terrain_uFace/2 == 2) {
    diffuse = vec3(0.9, 0.9, 1);
  }
  fragColor = vec4(luminance*diffuse, 1);
  fragDepth = length(vIn.c_pos);
}
