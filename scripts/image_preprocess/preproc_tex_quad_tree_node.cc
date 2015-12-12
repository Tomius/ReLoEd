// Copyright (c) 2015, Tamas Csala

#include <memory>
#include <iostream>
#include <algorithm>
#include <Magick++.h>
#include "./preproc_tex_quad_tree_node.h"

static const char* input_path = "/media/icecool/SSData/gmted75/height";

TexQuadTreeNode::TexQuadTreeNode(TexQuadTreeNode* parent,
                                 double x, double z, double sx, double sz,
                                 int level, unsigned index)
    : parent_(parent)
    , x_(x), z_(z)
    , sx_(sx), sz_(sz)
    , index_(index), level_(level)
{ }

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

unsigned short TexQuadTreeNode::FetchPixel(glm::ivec2 sample, glm::dvec2 diff) {
  return FetchPixel(sample.x, sample.y, std::abs(diff.x), std::abs(diff.y));
}

unsigned short TexQuadTreeNode::FetchPixel(int x, int z, double dx, double dz) {
  if (x < 0) {
    x += sx_;
  } else if (sx_ <= x) {
    x -= sx_;
  }

  if (z < 0) {
    z = std::abs(z);
  } else if (sz_ <= z) {
    z = sz_ - (z - sz_ + 1);
  }

  unsigned short ret = FetchPixelInternal(x, z, dx, dz);
  age();
  return ret;
}

unsigned short TexQuadTreeNode::FetchPixelInternal(int x, int z,
                                                   double dx, double dz) {
  // std::cout << x << ", " << z << ", " << dx << ", " << dz << std::endl;
  assert (int_left_x() <= x && x < int_right_x());
  assert (int_top_z() <= z && z < int_bottom_z());

  last_used_ = 0;

  load();
  int borderless_width = tex_w_ - 2*kBorderSize;
  int borderless_height = tex_h_ - 2*kBorderSize;
  glm::dvec2 pixel_coverage = glm::dvec2{sx_ / borderless_width,
                                         sz_ / borderless_height};
  // std::cout << dx << ", " << dz << std::endl;
  // std::cout << pixel_coverage << std::endl << std::endl;

  if (level_ == 0 || (pixel_coverage.x < dx && pixel_coverage.y < dz)) {
    int col = (x - int_left_x()) / pixel_coverage.x + kBorderSize;
    int row = (z - int_top_z()) / pixel_coverage.y + kBorderSize;
    int idx = row*tex_w_ + col;
    assert(0 <= idx && idx < data_.size());
    return data_[idx];
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
    return children_[idx]->FetchPixelInternal(x, z, dx, dz);
  }
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

unsigned short TexQuadTreeNode::SelectPixel(glm::dvec2 sample, glm::dvec2 diff) {
  return SelectPixel(sample.x, sample.y, std::abs(diff.x), std::abs(diff.y));
}

unsigned short TexQuadTreeNode::SelectPixel(double sample_x, double sample_z,
                                            double dx, double dz) {
  // sample_x /= dx / 2;
  // sample_z /= dz / 2;
  glm::ivec2 top_left = glm::ivec2(glm::floor(glm::dvec2{sample_x, sample_z}));
  glm::dvec2 fraction = glm::fract(glm::dvec2{sample_x, sample_z});

  double sum = 0.0;
  double sum_weight = 0.0;
  for (int x = -1; x <= 2; x++) {
    for (int y = -1; y <= 2; y++) {
      glm::ivec2 pos = top_left + glm::ivec2(x, y);
      double weight = CatMullRom(x - fraction.x)
                    * CatMullRom(-y + fraction.y);
      sum_weight += weight;
      sum += FetchPixel(pos.x, pos.y, dx, dz) * weight;
    }
  }

  return clamp(sum / sum_weight, 0, 65535);
}

