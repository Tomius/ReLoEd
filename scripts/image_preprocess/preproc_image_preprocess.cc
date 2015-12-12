#include <iostream>
#include "./preproc_settings.h"
#include "./preproc_tex_quad_tree_node.h"
#include "./preproc_cdlod_quad_tree_node.h"

int main(int argv, char** argc) {
  int tw = kInputWidth, th = kInputHeight;
  TexQuadTreeNode tex_node (nullptr, tw/2, th/2, tw, th, 9, 0);

  char face = argc[1][0];
  if ('0' <= face && face < '6') {
    CdlodQuadTreeNode node (kFaceSize/2, kFaceSize/2, CubeFace(face-'0'), kMaxLevel);
    node.GenerateImage(tex_node);
  } else {
    std::cout << "Invalid face parameter!";
  }
}
