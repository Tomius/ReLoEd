#include <iostream>
#include "./tex_quad_tree_node.h"
#include "./cdlod_quad_tree_node.h"

int main(int argv, char** argc) {
  int tw = 172800, th = 86400;
  TexQuadTreeNode tex_node (nullptr, tw/2, th/2, tw, th, 9, 0);

  char face = argc[1][0];
  if ('0' <= face && face < '6') {
    CdlodQuadTreeNode node (kFaceSize/2, kFaceSize/2, CubeFace(face-'0'), kMaxLevel);
    node.GenerateImage(tex_node);
  } else {
    std::cout << "Invalid face parameter!";
  }
}
