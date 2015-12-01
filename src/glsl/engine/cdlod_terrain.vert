#version 330

#export vec4 Terrain_modelPos(vec2 m_pos, vec4 render_data, out vec3 m_normal);
#export vec3 Terrain_worldPos(vec3 model_pos);
#export vec2 Terrain_texCoord(vec3 pos);
#export bool Terrain_isValid(vec3 m_pos);
#export float Terrain_getHeight(vec2 sample, out vec3 m_normal);

uniform int Terrain_uFace = 0;
uniform ivec2 Terrain_uTexSize;
uniform vec3 Terrain_uCamPos;
uniform float Terrain_uNodeDimension;
uniform float Terrain_uLodLevelDistanceMultiplier;
int Terrain_uNodeDimensionExp = int(round(log2(Terrain_uNodeDimension)));

uniform int Terrain_max_height;

float M_PI = 3.14159265359;
const float morph_end = 0.95, morph_start = 0.8;

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
    p.x * sqrt(1 - sqr(p.y)/2 - sqr(p.z)/2 + sqr(p.y*p.z)/3),
    p.y * sqrt(1 - sqr(p.z)/2 - sqr(p.x)/2 + sqr(p.z*p.x)/3),
    p.z * sqrt(1 - sqr(p.x)/2 - sqr(p.y)/2 + sqr(p.x*p.y)/3)
  );
}

vec3 Terrain_worldPos(vec3 pos) {
  float height = pos.y; pos.y = 0;
  pos = (pos - Terrain_uTexSize.x/2) / (Terrain_uTexSize.x/2);
  switch (Terrain_uFace) {
    case kPosX: pos = vec3(+pos.z, +pos.x, +pos.y); break;
    case kNegX: pos = vec3(+pos.z, +pos.x, -pos.y); break;
    case kPosY: pos = vec3(+pos.x, +pos.y, +pos.z); break;
    case kNegY: pos = vec3(+pos.x, -pos.y, +pos.z); break;
    case kPosZ: pos = vec3(+pos.y, +pos.z, +pos.x); break;
    case kNegZ: pos = vec3(-pos.y, +pos.z, +pos.x); break;
  }
  return (Terrain_radius + height) * Cubify(pos);
}

bool Terrain_isValid(vec3 m_pos) {
  return 0 <= m_pos.x && m_pos.x <= Terrain_uTexSize.x &&
         0 <= m_pos.z && m_pos.z <= Terrain_uTexSize.y;
}

float Terrain_estimateDistance(vec2 geom_pos) {
  float est_height = clamp(Terrain_cam_height, 0, Terrain_max_height);
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

vec4 Terrain_modelPos(vec2 m_pos, vec4 render_data, out vec3 m_normal) {
  vec2 offset = render_data.xy;
  float scale = render_data.z;
  float level = render_data.w;
  vec2 pos = Terrain_nodeLocal2Global(m_pos, offset, scale);
  int iteration_count = 0;

  float dist = Terrain_estimateDistance(pos);
  float next_level_size = pow(2, level+1)
                          * Terrain_uLodLevelDistanceMultiplier
                          * Terrain_uNodeDimension;
  float max_dist = morph_end * next_level_size;
  float start_dist = morph_start * next_level_size;
  float morph = smoothstep(start_dist, max_dist, dist);

  vec2 morphed_pos = Terrain_morphVertex(m_pos, morph);
  pos = Terrain_nodeLocal2Global(morphed_pos, offset, scale);
  dist = Terrain_estimateDistance(pos);

  while (dist > 1.5*next_level_size &&
         (iteration_count+1 < Terrain_uNodeDimensionExp)) {
    scale *= 2;
    next_level_size *= 2;
    iteration_count += 1;
    max_dist = morph_end * next_level_size;
    start_dist = morph_start * next_level_size;
    morph = smoothstep(start_dist, max_dist, dist);
    if (morph == 0.0) {
      break;
    }

    morphed_pos = Terrain_morphVertex(morphed_pos * 0.5, morph);
    pos = Terrain_nodeLocal2Global(morphed_pos, offset, scale);
    dist = Terrain_estimateDistance(pos);
  }

  float height = 0;
  m_normal = vec3(0, 1, 0);
  return vec4(pos.x, height, pos.y, iteration_count + 0/*morph*/);
}

vec2 Terrain_texCoord(vec3 pos) {
  return pos.xz / Terrain_uTexSize;
}

