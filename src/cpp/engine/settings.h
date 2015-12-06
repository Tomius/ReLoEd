// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_GLOBAL_HEGIHT_MAP_H_
#define ENGINE_GLOBAL_HEGIHT_MAP_H_

#include <climits>
#include "./transform.h"

namespace engine {

namespace Settings {
  static constexpr int kLevelOffset = 0;

  // CDLOD nodes' extent is (1 << kNodeDimensionExp)
  static constexpr int kNodeDimensionExp = 4;
  static_assert(4 <= kNodeDimensionExp && kNodeDimensionExp <= 8, "");

  static constexpr int kNodeDimension = 1 << kNodeDimensionExp;

  static constexpr int kTextureDimensionExp = 8;
  static constexpr int kTextureDimension = 1 << kTextureDimensionExp;

  static constexpr int kTextureBorderSize = 3;

  // The size of sphere for a CDLOD level is node size * this
  // It should be at least 2, but making it bigger makes distant
  // parts of the terrain appear with more detail.
  static constexpr double kLodLevelDistanceMultiplier = 2.0;
  static_assert(1 <= kLodLevelDistanceMultiplier, "");

  static constexpr double kTextureLevelDistanceMultiplier = 4.0;
  static_assert(1 <= kTextureLevelDistanceMultiplier, "");

  // Geometry subdivision. This practially contols zooming into the heightmap.
  // If for ex. this is three, that means that a 8x8 geometry (9x9 vertices)
  // corresponds to a 1x1 texture area (2x2 texels)
  static constexpr long kGeomDivBase = 2;
  static_assert(kGeomDivBase <= 2*kNodeDimensionExp, "");

  static constexpr long kGeomDiv = kGeomDivBase - kLevelOffset;

  // The resolution of the heightmap
  static constexpr long kFaceSize = 65536;

  // The radius of the sphere made of the heightmap
  static constexpr double kSphereRadius = kFaceSize / 2;

  static constexpr double kMtEverestHeight = 8848 * (kSphereRadius / 6371000);
  static constexpr double kHeightScale = 5;
  static constexpr double kMaxHeight = kHeightScale * kMtEverestHeight;

  static constexpr bool kWireFrame = false;

  // statistics
  extern size_t geom_nodes_count, texture_nodes_count;
};

}  // namespace engine

#endif
