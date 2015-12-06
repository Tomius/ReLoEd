// Copyright (c) 2015, Tamas Csala

#ifndef ENGINE_CDLOD_QUAD_TREE_NODE_H_
#define ENGINE_CDLOD_QUAD_TREE_NODE_H_

#include <memory>
#include "./tex_quad_tree_node.h"
#include "./cube2sphere.h"

class CdlodQuadTreeNode {
 public:
  CdlodQuadTreeNode(double x, double z, CubeFace face, int level);

  double scale() const { return pow(2, level_); }
  double size() const { return kTexNodeDimension * scale(); }

  void GenerateImage(TexQuadTreeNode& texture);

 private:
  double x_, z_;
  CubeFace face_;
  int level_;
  std::unique_ptr<CdlodQuadTreeNode> children_[4];

  void initChild(int i);
};

#endif
