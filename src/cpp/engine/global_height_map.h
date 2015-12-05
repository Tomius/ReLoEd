// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_GLOBAL_HEGIHT_MAP_H_
#define ENGINE_GLOBAL_HEGIHT_MAP_H_

#include <climits>
#include "./transform.h"

namespace engine {

namespace Settings {
  static constexpr int level_offset = 0;

  // CDLOD nodes' extent is (1 << node_dimension_exp)
  static constexpr int node_dimension_exp = 4;
  static_assert(4 <= node_dimension_exp && node_dimension_exp <= 8, "");

  static constexpr int node_dimension = 1 << node_dimension_exp;

  static constexpr int kTextureDimensionExp = 8;
  static constexpr int kTextureDimension = 1 << kTextureDimensionExp;

  static constexpr int kTextureBorderSize = 3;

  // The size of sphere for a CDLOD level is node size * this
  // It should be at least 2, but making it bigger makes distant
  // parts of the terrain appear with more detail.
  static constexpr double lod_level_distance_multiplier = 3.0;
  static_assert(1 <= lod_level_distance_multiplier, "");

  static constexpr double texture_level_distance_multiplier = 4.0;
  static_assert(1 <= texture_level_distance_multiplier, "");

  // Geometry subdivision. This practially contols zooming into the heightmap.
  // If for ex. this is three, that means that a 8x8 geometry (9x9 vertices)
  // corresponds to a 1x1 texture area (2x2 texels)
  static constexpr long geom_div_base = 2;
  static_assert(geom_div_base <= 2*node_dimension_exp, "");

  static constexpr long geom_div = geom_div_base - level_offset;

  // The resolution of the heightmap
  static constexpr long face_size = 65536;

  // The radius of the sphere made of the heightmap
  static constexpr double sphere_radius = face_size / 2;

  static constexpr double mt_everest_height = 8848 * (sphere_radius / 6371000);
  static constexpr double height_scale = 5;
  static constexpr double max_height = height_scale * mt_everest_height;

  static constexpr bool wire_frame = false;

  // statistics
  extern size_t geom_nodes_count, texture_nodes_count;
};

}  // namespace engine

#endif
