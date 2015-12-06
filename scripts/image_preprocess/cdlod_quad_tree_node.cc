// Copyright (c) 2015, Tamas Csala

#include <iomanip>
#include <iostream>
#include <algorithm>
#include "./cube2sphere.h"
#include "./cdlod_quad_tree_node.h"

static long image_counter = 0;
static time_t start_timer, current_timer;

CdlodQuadTreeNode::CdlodQuadTreeNode(double x, double z,
                                     CubeFace face, int level)
    : x_(x), z_(z), face_(face), level_(level)
{ }

void CdlodQuadTreeNode::initChild(int i) {
  assert (0 <= i && i <= 3);

  double s4 = size()/4;
  if (i == 0) {
    children_[0] = std::make_unique<CdlodQuadTreeNode>(x_-s4, z_+s4, face_, level_-1);
  } else if (i == 1) {
    children_[1] = std::make_unique<CdlodQuadTreeNode>(x_+s4, z_+s4, face_, level_-1);
  } else if (i == 2) {
    children_[2] = std::make_unique<CdlodQuadTreeNode>(x_-s4, z_-s4, face_, level_-1);
  } else if (i == 3) {
    children_[3] = std::make_unique<CdlodQuadTreeNode>(x_+s4, z_-s4, face_, level_-1);
  }
}

void CdlodQuadTreeNode::GenerateImage(TexQuadTreeNode& texture) {
  if (level_ < 1) {
    return;
  }

  if (image_counter == 0) {
    time(&start_timer);
  }
  time(&current_timer);

  if (++image_counter % 10 == 0) {
    std::cout.precision(6);
    double percent = image_counter * 100.0 / kImageCount;
    double elapsed_time = difftime(current_timer, start_timer) / 60;
    double remaining = elapsed_time * (100-percent) / percent;
    std::cout << '[' << std::setw(8) << percent << " %] " <<
      "elapsed: " << elapsed_time << "m, remaining: " << remaining << "m" << std::endl;
  }

  int w = kTexNodeDimension+2*kBorderSize, h = w;
  unsigned short *image = new unsigned short[w*h]{};
  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      double left_x = x_ - size()/2, top_z = z_ - size()/2;

      glm::dvec3 sample{
        left_x + (x-kBorderSize) * size() / (w-2*kBorderSize), 0,
        top_z  + (y-kBorderSize) * size() / (h-2*kBorderSize)
      };
      glm::dvec2 plane_sample = Cube2Plane(sample, face_, kFaceSize);


      // unsigned short value = texture.FetchPixel (plane_sample.x, plane_sample.y, level_);
      unsigned short value = texture.SelectPixel (plane_sample.x, plane_sample.y, level_);
      image[y*w + x] = value;
      // image[y*w + x] = pow(value / 65535.0, 1/2.2) * 65535.0;
    }
  }

  Magick::Image out;
  out.read(w, h, "I", Magick::ShortPixel, image);
  delete[] image;

  std::string dir = output_dir + "/" + std::to_string(int(face_)) +
                                 "/" + std::to_string(level_) +
                                 "/" + std::to_string(long(x_));
  std::string filename = std::to_string(long(z_)) + ".png";

  int ret = system(("mkdir -p " + dir).c_str());
  assert (ret == 0);

  out.matte(false);
  out.quality(100);
  out.defineValue("png", "color-type", "0");
  out.defineValue("png", "bit-depth", "16");
  out.write(dir + "/" + filename);

  for (int i = 0; i < 4; ++i) {
    if (!children_[i]) {
      initChild(i);
    }
    children_[i]->GenerateImage(texture);
    children_[i].reset();
  }
}
