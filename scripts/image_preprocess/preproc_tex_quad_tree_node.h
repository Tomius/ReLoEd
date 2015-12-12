// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_NODE_H_

#include <memory>
#include <cassert>
#include <Magick++.h>
#include <glm/glm.hpp>
#include "./preproc_settings.h"

using TexelData = unsigned short;

class TexQuadTreeNode {
 public:
  TexQuadTreeNode(TexQuadTreeNode* parent,
                  double center_x, double center_z,
                  double size_x, double size_z,
                  int mip_level, unsigned index);

  void load();

  void age();

  double center_x() const { return x_; }
  double center_z() const { return z_; }
  double size_x() const { return sx_; }
  double size_z() const { return sz_; }

  int int_left_x() const { return int(x_) - int(sx_)/2; }
  double left_x() const {
    return level_ >= 0 ? int_left_x() : x_ - sx_ / 2.0;
  }
  int int_right_x() const { return int_left_x() + int(sx_); }
  double right_x() const { return left_x() + sx_; }

  int int_top_z() const { return int(z_) - int(sz_)/2; }
  double top_z() const {
    return level_ >= 0 ? int_top_z() : z_ - sz_ / 2.0;
  }
  int int_bottom_z() const { return int_top_z() + int(sz_); }
  double bottom_z() const { return top_z() + sz_; }

  int level() const { return level_; }
  int index() const { return index_; }

  std::string map_path(const char* base_path) const;
  std::string height_map_path() const;

  bool is_image_loaded() const { return !data_.empty(); }
  int last_used() const { return last_used_; }
  unsigned data_start_offset() const { return data_start_offset_; }
  const std::vector<TexelData>& data() const { return data_; }
  TexQuadTreeNode* parent() const { return parent_; }
  TexQuadTreeNode* getChild(int i) const {
    assert(0 <= i && i < 4); return children_[i].get();
  }

  unsigned short SelectPixel(glm::dvec2 sample, glm::dvec2 diff);
  unsigned short FetchPixel(glm::ivec2 sample, glm::dvec2 diff);

 private:
  unsigned short SelectPixel(double x, double z, double dx, double dz);
  unsigned short FetchPixel(int x, int z, double dx, double dz);
  unsigned short FetchPixelInternal(int x, int z, double dx, double dz);

  TexQuadTreeNode* parent_;
  double x_, z_, sx_, sz_;
  unsigned tex_w_, tex_h_, index_, data_start_offset_ = 0;
  int level_;
  std::unique_ptr<TexQuadTreeNode> children_[4];
  std::vector<TexelData> data_;

  int last_used_ = 0;
  static const int kTimeToLiveInMemory = 1 << 12;

  template<typename T>
  void initChildInternal(int i);

  void initChild(int i);
};

#endif
