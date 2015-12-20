// Copyright (c) 2015, Tamas Csala

#include <limits>
#include <memory>
#include <iostream>
#include <algorithm>
#include <Magick++.h>
#include "./preproc_tex_quad_tree_node.h"

TexQuadTreeNode::TexQuadTreeNode(TexQuadTreeNode* parent,
                                 double x, double y, double sx, double sy,
                                 int level, unsigned index)
    : parent_(parent)
    , x_(x), y_(y)
    , sx_(sx), sy_(sy)
    , index_(index), level_(level)
{ }

std::string TexQuadTreeNode::texture_path() const {
  char file_path[200];
  int tx = int_left_x(), ty = int_top_y();
  sprintf(file_path, "%s/%d/%d/%d.png", kInputDir.c_str(), level_, tx, ty);

  return file_path;
}

void TexQuadTreeNode::load() {
  if (is_image_loaded()) {
    return;
  }

  Magick::Image image{texture_path()};
  tex_w_ = image.columns();
  tex_h_ = image.rows();
  data_.resize(tex_w_*tex_h_);

#ifdef HEIGHTMAP
  image.write(0, 0, tex_w_, tex_h_, "R", MagickCore::ShortPixel, data_.data());
#else
  image.write(0, 0, tex_w_, tex_h_, "RGB", MagickCore::CharPixel, data_.data());
#endif
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
  T top_sy = T(sy_)/2;
  T bottom_sy = T(sy_) - T(sy_)/2;

  T left_cx = T(x_) - (left_sx - left_sx/2);
  T right_cx = T(x_) + right_sx/2;
  T top_cy = T(y_) - (top_sy - top_sy/2);
  T bottom_cy = T(y_) + bottom_sy/2;

  switch (i) {
    case 0: { // top left
      children_[0] = std::make_unique<TexQuadTreeNode>(
          this, left_cx, top_cy, left_sx, top_sy, level_-1, i);
    } break;
    case 1: { // top right
      children_[1] = std::make_unique<TexQuadTreeNode>(
          this, right_cx, top_cy, right_sx, top_sy, level_-1, i);
    } break;
    case 2: { // bottom left
      children_[2] = std::make_unique<TexQuadTreeNode>(
          this, left_cx, bottom_cy, left_sx, bottom_sy, level_-1, i);
    } break;
    case 3: { // bottom right
      children_[3] = std::make_unique<TexQuadTreeNode>(
          this, right_cx, bottom_cy, right_sx, bottom_sy, level_-1, i);
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

static float BellFunc(float x) {
  float f = x * 0.75; // Converting -2 to +2 to -1.5 to +1.5
  if (f > -1.5 && f < -0.5) {
    return 0.5 * pow(f + 1.5, 2.0);
  } else if (f > -0.5 && f < 0.5) {
    return 3.0 / 4.0 - (f*f);
  } else if (f > 0.5 && f < 1.5) {
    return 0.5 * pow(f - 1.5, 2.0);
  } else {
    return 0.0;
  }
}

static float CatMullRom(float x) {
  const float B = 0.0;
  const float C = 0.5;
  float f = std::abs(x);

  if (f < 1.0) {
    return ((12 - 9*B - 6*C) * (f*f*f) +
            (-18 + 12*B + 6*C) * (f*f) +
            (6 - 2*B)) / 6.0;
  } else if (1.0 <= f && f < 2.0) {
    return ((-B - 6*C) * (f*f*f) +
            (6*B + 30*C) * (f*f) +
            (-12*B - 48*C) * f +
             8*B + 24*C) / 6.0;
  } else {
    return 0.0;
  }
}

TexelData TexQuadTreeNode::FetchPixel(glm::dvec2 sample, glm::dvec2 diff) {
  return FetchPixel(sample.x, sample.y, std::abs(diff.x), std::abs(diff.y));
}

TexelData TexQuadTreeNode::FetchPixel(double x, double y,
                                           double dx, double dy) {
  if (x < 0) {
    x += sx_;
  } else if (sx_ <= x) {
    x -= sx_;
  }

  if (y < 0) {
    y = std::abs(y);
  } else if (sy_ <= y) {
    y = sy_ - (y - sy_ + 1);
  }

  TexelData ret = FetchPixelInternal(x, y, dx, dy);
  age();
  return ret;
}

TexelData TexQuadTreeNode::FetchPixelInternal(double x, double y,
                                              double dx, double dy) {
  assert (int_left_x() <= x && x < int_right_x());
  assert (int_top_y() <= y && y < int_bottom_y());

  last_used_ = 0;

  load();
  int borderless_width = tex_w_ - 2*kBorderSize;
  int borderless_height = tex_h_ - 2*kBorderSize;
  glm::dvec2 pixel_coverage = glm::dvec2{sx_ / borderless_width,
                                         sy_ / borderless_height};

  if (level_ == 0 || (pixel_coverage.x < dx && pixel_coverage.y < dy)) {
    glm::ivec2 top_left = glm::ivec2(glm::floor(glm::dvec2{x, y}));
    glm::dvec2 fraction = glm::fract(glm::dvec2{x, y});

#ifdef HEIGHTMAP
    double sum = 0.0;
#else
    double sum_r = 0.0, sum_g = 0.0, sum_b = 0.0;
#endif

    double sum_weight = 0.0;
    for (int xx = -1; xx <= 2; xx++) {
      for (int yy = -1; yy <= 2; yy++) {
        glm::ivec2 pos = top_left + glm::ivec2(xx, yy);

        int col = (pos.x - int_left_x()) / pixel_coverage.x + kBorderSize;
        int row = (pos.y - int_top_y()) / pixel_coverage.y + kBorderSize;
        int idx = row*tex_w_ + col;
        assert(0 <= idx && idx < data_.size());
        TexelData texel_value = data_[idx];

        double weight = CatMullRom(xx - fraction.x)
                      * CatMullRom(-yy + fraction.y);
        sum_weight += weight;

#ifdef HEIGHTMAP
        sum += texel_value * weight;
#else
        sum_r += texel_value.r * weight;
        sum_g += texel_value.g * weight;
        sum_b += texel_value.b * weight;
#endif
      }
    }

#ifdef HEIGHTMAP
    return static_cast<TexelData>(clamp(round(sum / sum_weight),
        std::numeric_limits<TexelData>::min(),
        std::numeric_limits<TexelData>::max()));
#else
    return TexelData{
      static_cast<unsigned char>(clamp(round(sum_r / sum_weight),
        std::numeric_limits<unsigned char>::min(),
        std::numeric_limits<unsigned char>::max())),
      static_cast<unsigned char>(clamp(round(sum_g / sum_weight),
        std::numeric_limits<unsigned char>::min(),
        std::numeric_limits<unsigned char>::max())),
      static_cast<unsigned char>(clamp(round(sum_b / sum_weight),
        std::numeric_limits<unsigned char>::min(),
        std::numeric_limits<unsigned char>::max()))
    };
#endif
  } else {
    int idx;
    if (x < x_) {
      idx = y < y_ ? 0 : 2;
    } else {
      idx = y < y_ ? 1 : 3;
    }

    if (!children_[idx]) {
      initChild(idx);
    }
    return children_[idx]->FetchPixelInternal(x, y, dx, dy);
  }
}

