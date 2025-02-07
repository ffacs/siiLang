#pragma once
#include "IR/IR.h"

namespace SiiIR {

struct BasicGroup {
  std::vector<SiiIRCode*> codes_;
};

using BasicGroupPtr = std::shared_ptr<BasicGroup>;

struct BasicGroupNode {
  BasicGroupPtr basic_group_;
  std::vector<BasicGroupNode *> precedes_;
  std::vector<BasicGroupNode *> follows_;
  BasicGroupNode() : basic_group_(std::make_shared<BasicGroup>()) {}
};

using BasicGroupNodePtr = std::shared_ptr<BasicGroupNode>;

struct CFG {
  std::shared_ptr<std::vector<SiiIRCodePtr>> codes_in_order_;
  std::vector<BasicGroupNodePtr> basic_groups_;
  BasicGroupNode* entry_;
};

using CFGPtr = std::shared_ptr<CFG>;

CFGPtr BuildCFG(std::shared_ptr<std::vector<SiiIRCodePtr>> codes);
// TODO merge BasicGroup with its following BasicGroup when there is only one

} // namespace SiiIR