#include "global_height_map.h"

namespace engine {

const char *GlobalHeightMap::height_texture_base_path =
  "/media/icecool/SSData/gmted75/height";

const char *GlobalHeightMap::dx_texture_base_path =
  // "/media/icecool/Data/onlab/src/resources/gmted75/normals/dx";
  "/media/icecool/SSData/gmted75/dx";

const char *GlobalHeightMap::dy_texture_base_path =
  // "/media/icecool/Data/onlab/src/resources/gmted75/normals/dy";
  "/media/icecool/SSData/gmted75/dy";

bool GlobalHeightMap::wire_frame = false;

size_t GlobalHeightMap::geom_nodes_count = 0;
size_t GlobalHeightMap::texture_nodes_count = 0;
size_t GlobalHeightMap::gpu_mem_usage = 0;

}
