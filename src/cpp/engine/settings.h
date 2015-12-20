// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_GLOBAL_HEGIHT_MAP_H_
#define ENGINE_GLOBAL_HEGIHT_MAP_H_

#include <climits>
#include "./transform.h"

namespace engine {

namespace Settings {

  static constexpr int kNodeDimensionExp = 6;
  static constexpr int kNodeDimension = 1 << kNodeDimensionExp;

  static constexpr int kTextureDimensionExp = 8;
  static constexpr int kTextureDimension = 1 << kTextureDimensionExp;

  static constexpr int kTexDimOffset = kTextureDimensionExp - kNodeDimensionExp;

  static constexpr int kElevationTexBorderSize = 3;
  static constexpr int kElevationTexSizeWithBorders =
      kTextureDimension + 2*kElevationTexBorderSize;
  static constexpr int kDiffuseTexBorderSize = 2;
  static constexpr int kDiffuseTexSizeWithBorders =
      kTextureDimension + 2*kDiffuseTexBorderSize;

  static constexpr int kLevelOffset = 0;
  static constexpr int kDiffuseToElevationLevelOffset = 1;
  static constexpr int kNormalToGeometryLevelOffset = 2;

  static constexpr double kSmallestGeometryLodDistance = 2*kNodeDimension;
  static constexpr double kSmallestTextureLodDistance =
    kSmallestGeometryLodDistance * (1 << kNormalToGeometryLevelOffset);

  // Geometry subdivision. This practially contols zooming into the heightmap.
  // If for ex. this is three, that means that a 8x8 geometry (9x9 vertices)
  // corresponds to a 1x1 texture area (2x2 texels)
  static constexpr long kGeomDiv = 2;

  // The resolution of the heightmap
  static constexpr long kFaceSize = 65536;

  // The radius of the sphere made of the heightmap
  static constexpr double kSphereRadius = kFaceSize / 2;

  static constexpr double kMtEverestHeight = 8848 * (kSphereRadius / 6371000);
  static constexpr double kHeightScale = 3;
  static constexpr double kMaxHeight = kHeightScale * kMtEverestHeight;

  static constexpr bool kWireFrame = false;

  // statistics
  extern bool render, update;
  extern size_t geom_nodes_count, texture_nodes_count;

  static_assert(3 <= kNodeDimensionExp && kNodeDimensionExp <= 8, "");
  static_assert(kNodeDimension <= kSmallestGeometryLodDistance, "");
  static_assert(0 <= kNormalToGeometryLevelOffset, "");
  static_assert(kNodeDimensionExp + kNormalToGeometryLevelOffset <= kTextureDimensionExp, "");
  static_assert(kTextureDimension <= kSmallestTextureLodDistance, "");
  static_assert(kSmallestGeometryLodDistance <= kSmallestTextureLodDistance, "");
  static_assert(kGeomDiv <= 2*kNodeDimensionExp, "");
};

}  // namespace engine

#endif
