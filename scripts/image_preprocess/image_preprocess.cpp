#include <iostream>
#include "tex_quad_tree_node.h"

int main() {
  int tw = 172800, th = 86400;
  TexQuadTreeNode root (nullptr, tw/2, th/2, tw, th, 9, 0);

  int w = 337+6, h = 168+6;
  unsigned short *image = new unsigned short[w*h];
  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      unsigned short value = root.SelectPixel ((x-3)*(tw/(w-6)), (y-3)*(th/(h-6)), 9);
      image[y*w + x] = value;
    }
  }

  Magick::Image out;
  out.matte(false);
  out.quality(100);
  out.defineValue("png", "bit-depth", "16");
  out.read(w, h, "I", Magick::ShortPixel, image);
  out.write("out.png");
}
