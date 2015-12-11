// Copyright (c) 2015, Tamas Csala

#version 330

#export vec3 Terrain_worldPos(vec3 pos, int face);
#export float Terrain_radius();

const int kPosX = 0;
const int kNegX = 1;
const int kPosY = 2;
const int kNegY = 3;
const int kPosZ = 4;
const int kNegZ = 5;

uniform ivec2 Terrain_uTexSize;

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

vec3 Terrain_worldPos(vec3 pos, int face) {
  float height = pos.y; pos.y = 0;
  pos = (pos - Terrain_uTexSize.x/2) / (Terrain_uTexSize.x/2);
  switch (face) {
    case kPosX: pos = vec3(-pos.y, -pos.z, -pos.x); break;
    case kNegX: pos = vec3(+pos.y, -pos.z, +pos.x); break;
    case kPosY: pos = vec3(-pos.z, -pos.y, +pos.x); break;
    case kNegY: pos = vec3(+pos.z, +pos.y, +pos.x); break;
    case kPosZ: pos = vec3(-pos.x, -pos.z, +pos.y); break;
    case kNegZ: pos = vec3(+pos.x, -pos.z, -pos.y); break;
  }
  return (Terrain_radius() + height) * Cubify(pos);
}

float Terrain_radius() {
  return Terrain_uTexSize.x / 2;
}
