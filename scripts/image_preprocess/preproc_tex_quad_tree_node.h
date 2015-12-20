// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_TEXTURE_TEX_QUAD_TREE_NODE_H_

#include <memory>
#include <cassert>
#include <Magick++.h>
#include <glm/glm.hpp>
#include "./preproc_settings.h"

#ifdef HEIGHTMAP
  using TexelData = unsigned short;
#else
  struct TexelData {
    unsigned char r, g, b;
  };
#endif

class TexQuadTreeNode {
 public:
  TexQuadTreeNode(TexQuadTreeNode* parent,
                  double center_x, double center_y,
                  double size_x, double size_y,
                  int mip_level, unsigned index);

  void load();

  void age();

  double center_x() const { return x_; }
  double center_y() const { return y_; }
  double size_x() const { return sx_; }
  double size_y() const { return sy_; }

  int int_left_x() const { return int(x_) - int(sx_)/2; }
  double left_x() const {
    return level_ >= 0 ? int_left_x() : x_ - sx_ / 2.0;
  }
  int int_right_x() const { return int_left_x() + int(sx_); }
  double right_x() const { return left_x() + sx_; }

  int int_top_y() const { return int(y_) - int(sy_)/2; }
  double top_y() const {
    return level_ >= 0 ? int_top_y() : y_ - sy_ / 2.0;
  }
  int int_bottom_y() const { return int_top_y() + int(sy_); }
  double bottom_y() const { return top_y() + sy_; }

  int level() const { return level_; }
  int index() const { return index_; }

  std::string texture_path() const;

  bool is_image_loaded() const { return !data_.empty(); }
  int last_used() const { return last_used_; }
  unsigned data_start_offset() const { return data_start_offset_; }
  const std::vector<TexelData>& data() const { return data_; }
  TexQuadTreeNode* parent() const { return parent_; }
  TexQuadTreeNode* getChild(int i) const {
    assert(0 <= i && i < 4); return children_[i].get();
  }

  TexelData FetchPixel(glm::dvec2 sample, glm::dvec2 diff);

 private:
  TexelData FetchPixel(double x, double y, double dx, double dy);
  TexelData FetchPixelInternal(double x, double y, double dx, double dy);

  TexQuadTreeNode* parent_;
  double x_, y_, sx_, sy_;
  unsigned tex_w_, tex_h_, index_, data_start_offset_ = 0;
  int level_;
  std::unique_ptr<TexQuadTreeNode> children_[4];
  std::vector<TexelData> data_;

  int last_used_ = 0;
  static const int kTimeToLiveInMemory = 1 << 16;

  template<typename T>
  void initChildInternal(int i);

  void initChild(int i);
};

#endif
