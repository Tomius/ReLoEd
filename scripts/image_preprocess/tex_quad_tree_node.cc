// Copyright (c) 2015, Tamas Csala

#include <memory>
#include <iostream>
#include <algorithm>
#include <Magick++.h>
#include "./tex_quad_tree_node.h"

static const char* input_path = "/media/icecool/SSData/gmted75/height";
const int kBorderSize = 3;

TexQuadTreeNode::TexQuadTreeNode(TexQuadTreeNode* parent,
                                 double x, double z, double sx, double sz,
                                 int level, unsigned index)
    : parent_(parent)
    , x_(x), z_(z)
    , sx_(sx), sz_(sz)
    , index_(index), level_(level) {}

std::string TexQuadTreeNode::map_path(const char* base_path) const {
  char file_path[200];
  int tx = int_left_x(), ty = int_top_z();
  sprintf(file_path, "%s/%d/%d/%d.png", base_path, level_, tx, ty);

  return file_path;
}

std::string TexQuadTreeNode::height_map_path() const {
  return map_path(input_path);
}

void TexQuadTreeNode::load() {
  if (is_image_loaded()) {
    return;
  }

  { // height tex
    Magick::Image height{height_map_path()};
    tex_w_ = height.columns();
    tex_h_ = height.rows();
    data_.resize(tex_w_*tex_h_);

    height.write(0, 0, tex_w_, tex_h_, "R", MagickCore::ShortPixel, data_.data());
  }
}

void TexQuadTreeNode::age() {
  last_used_++;

  for (auto& child : children_) {
    if (child) {
      // unload child if its age would exceed the ttl
      if (kTimeToLiveInMemory <= child->last_used_) {
        child.reset();
      } else {
        child->age();
      }
    }
  }
}

template<typename T>
void TexQuadTreeNode::initChildInternal(int i) {
  T left_sx = T(sx_)/2;
  T right_sx = T(sx_) - T(sx_)/2;
  T top_sz = T(sz_)/2;
  T bottom_sz = T(sz_) - T(sz_)/2;

  T left_cx = T(x_) - (left_sx - left_sx/2);
  T right_cx = T(x_) + right_sx/2;
  T top_cz = T(z_) - (top_sz - top_sz/2);
  T bottom_cz = T(z_) + bottom_sz/2;

  switch (i) {
    case 0: { // top left
      children_[0] = std::make_unique<TexQuadTreeNode>(
          this, left_cx, top_cz, left_sx, top_sz, level_-1, i);
    } break;
    case 1: { // top right
      children_[1] = std::make_unique<TexQuadTreeNode>(
          this, right_cx, top_cz, right_sx, top_sz, level_-1, i);
    } break;
    case 2: { // bottom left
      children_[2] = std::make_unique<TexQuadTreeNode>(
          this, left_cx, bottom_cz, left_sx, bottom_sz, level_-1, i);
    } break;
    case 3: { // bottom right
      children_[3] = std::make_unique<TexQuadTreeNode>(
          this, right_cx, bottom_cz, right_sx, bottom_sz, level_-1, i);
    } break;
    default: {
      throw std::out_of_range("Tried to index "
          + std::to_string(i) + "th child of a quadtree node.");
    }
  }
}

void TexQuadTreeNode::initChild(int i) {
  initChildInternal<int>(i);
}

unsigned short TexQuadTreeNode::SelectPixel(int x, int z, int level) {
  if (x < 0) {
    x += sx_;
  } else if (sx_ <= x) {
    x -= sz_;
  }

  if (z < 0) {
    z = std::abs(z);
  } else if (sz_ < z) {
    z = sz_ - (z - sz_);
  }

  unsigned short ret = SelectPixelInternal(x, z, level);
  age();
  return ret;
}

unsigned short TexQuadTreeNode::SelectPixelInternal(int x, int z, int level) {
  assert (level_ >= level);
  assert (int_left_x() <= x && x < int_right_x());
  assert (int_top_z() <= z && z < int_bottom_z());

  last_used_ = 0;

  if (level == level_) {
    load();
    int col = (x - int_left_x()) / sx_ * (tex_w_-2*kBorderSize) + kBorderSize;
    int row = (z - int_top_z()) / sz_ * (tex_h_-2*kBorderSize) + kBorderSize;
    return data_[row*tex_w_ + col];
  } else {
    int idx;
    if (x < x_) {
      idx = z < z_ ? 0 : 2;
    } else {
      idx = z < z_ ? 1 : 3;
    }

    if (!children_[idx]) {
      initChild(idx);
    }
    return children_[idx]->SelectPixelInternal(x, z, level);
  }
}

