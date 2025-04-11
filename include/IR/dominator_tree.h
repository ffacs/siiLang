#pragma once

#include "IR/function.h"

namespace SiiIR {
struct DominatorTreeNode {
  DominatorTreeNode*              parent_ = nullptr;
  size_t                          level   = 0;
  std::vector<DominatorTreeNode*> children_;
  BasicGroup*                     basic_group_;
  DominatorTreeNode(BasicGroup* basic_group_node)
      : basic_group_(basic_group_node) {}
  DominatorTreeNode() = default;
};
using DominatorTreeNodePtr = std::shared_ptr<DominatorTreeNode>;

struct DominatorTree {
  std::vector<DominatorTreeNodePtr> nodes_;
  DominatorTreeNode*                root_;

  DominatorTree(DominatorTreeNodePtr root)
      : root_(root.get()) {
    nodes_.push_back(root);
  }
};

using DominatorTreePtr = std::shared_ptr<DominatorTree>;

DominatorTreePtr BuildDominatorTree(FunctionPtr func);
}  // namespace SiiIR
