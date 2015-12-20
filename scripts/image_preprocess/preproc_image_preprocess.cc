#include <iostream>
#include "./preproc_settings.h"
#include "./preproc_tex_quad_tree_node.h"
#include "./preproc_cdlod_quad_tree_node.h"

int main(int argv, char** argc) {
  int tw = kInputWidth, th = kInputHeight;
  TexQuadTreeNode tex_node (nullptr, tw/2, th/2, tw, th, kInputMaxLevel, 0);

  if (argv != 2) {
    std::cerr << "Face parameter must be specified" << std::endl;
    return -1;
  }

  char face = argc[1][0];
  if ('0' <= face && face < '6') {
    CdlodQuadTreeNode node (kFaceSize/2, kFaceSize/2, CubeFace(face-'0'), kMaxLevel);
    node.GenerateImage(tex_node);
  } else {
    std::cout << "Invalid face parameter!";
    return -1;
  }

  return 0;
}
