// Copyright (c) 2015, Tamas Csala

#version 330
#extension GL_ARB_bindless_texture : require

#export vec4 Terrain_modelPos(vec2 m_pos, vec4 render_data, out vec3 m_normal);
#export vec3 Terrain_worldPos(vec3 model_pos);
#export vec2 Terrain_texCoord(vec3 pos);

in uvec2 Terrain_aCurrentGeometryTextureId;
in vec3 Terrain_aCurrentGeometryTexturePosAndSize;
in uvec2 Terrain_aNextGeometryTextureId;
in vec3 Terrain_aNextGeometryTexturePosAndSize;

uniform int Terrain_uFace;
uniform int Terrain_uMaxLoadLevel;
uniform int Terrain_uTextureDimensionWBorders;
uniform ivec2 Terrain_uTexSize;
uniform vec3 Terrain_uCamPos;
uniform float Terrain_uSmallestGeometryLodDistance;

uniform int Terrain_uMaxHeight;

const float kMorphEnd = 0.95, kMorphStart = 0.85;

float Terrain_radius = Terrain_uTexSize.x / 2;
float Terrain_cam_height = length(Terrain_uCamPos) - Terrain_radius;

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

float Terrain_estimateDistance(vec2 geom_pos) {
  float est_height = clamp(Terrain_cam_height, 0, Terrain_uMaxHeight);
  vec3 est_pos = vec3(geom_pos.x, est_height, geom_pos.y);
  vec3 est_diff = Terrain_uCamPos - Terrain_worldPos(est_pos);
  return length(est_diff);
}

vec2 Terrain_morphVertex(vec2 vertex, float morph) {
  vec2 frac_part = fract(vertex * 0.5) * 2.0;
  return (vertex - frac_part * morph);
}

vec2 Terrain_nodeLocal2Global(vec2 node_coord, vec2 offset, float scale) {
  vec2 pos = offset + scale * node_coord;
  return pos;
}

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

float Terrain_getHeightInternal(vec2 pos, uvec2 texid, vec3 texPosAndSize) {
  vec2 sample = (pos - texPosAndSize.xy) / texPosAndSize.z;
  sample += 0.5 / Terrain_uTextureDimensionWBorders;
  float normalized_height = textureBicubic(sampler2D(texid), sample).r;
  return normalized_height * Terrain_uMaxHeight;
}

float Terrain_getHeight(vec2 pos, float level, float morph) {
  float height0 =
    Terrain_getHeightInternal(pos, Terrain_aCurrentGeometryTextureId,
                              Terrain_aCurrentGeometryTexturePosAndSize);
  if (morph == 0.0 || level <= 0) {
    return height0;
  }

  float height1 =
    Terrain_getHeightInternal(pos, Terrain_aNextGeometryTextureId,
                              Terrain_aNextGeometryTexturePosAndSize);

  return mix(height0, height1, morph);
}

vec4 Terrain_modelPos(vec2 m_pos, vec4 render_data, out vec3 m_normal) {
  vec2 offset = render_data.xy;
  float scale = render_data.z;
  float level = render_data.w;
  vec2 pos = Terrain_nodeLocal2Global(m_pos, offset, scale);
  float dist = Terrain_estimateDistance(pos);
  float morph = 0;

  if (level < Terrain_uMaxLoadLevel) {
    float next_level_size =
        2 * scale * Terrain_uSmallestGeometryLodDistance;
    float max_dist = kMorphEnd * next_level_size;
    float start_dist = kMorphStart * next_level_size;
    morph = smoothstep(start_dist, max_dist, dist);

    vec2 morphed_pos = Terrain_morphVertex(m_pos, morph);
    pos = Terrain_nodeLocal2Global(morphed_pos, offset, scale);
    dist = Terrain_estimateDistance(pos);
  }

  float height = Terrain_getHeight(pos, level, morph);
  m_normal = vec3(0, 1, 0);
  return vec4(pos.x, height, pos.y, morph);
}

vec2 Terrain_texCoord(vec3 pos) {
  return pos.xz / Terrain_uTexSize;
}

