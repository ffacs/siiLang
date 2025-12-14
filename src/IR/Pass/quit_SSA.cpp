#include "IR/Pass/quit_SSA.h"

namespace SiiIR {

void QuitSSAPass::run(FunctionPtr& func) {
  for (auto& bg : func->basic_groups_) {
    auto& code_list = bg->codes_;
    for (auto iter = code_list.begin(); iter != code_list.end(); ++iter) {
      auto& code = *iter;
      if (code.kind_ != SiiIRCodeKind::PHI) {
        continue;
      }
      auto& phi_code = static_cast<SiiIRPhi&>(code);
      for(size_t i = 0; i < phi_code.src_list_.size(); i++) {
        ValuePtr    src_value = phi_code.src_list_[i]->value_;
        BasicGroup* pred_bg   = bg->precedes_[i];
        // Insert a assign instruction at the end of pred_bg
        std::shared_ptr<SiiIRAssign> assign
            = std::make_shared<SiiIRAssign>(iter.shared(), src_value);
        auto& pred_code_list = pred_bg->codes_;
        pred_code_list.insert_before(--pred_code_list.end(), assign);
      }
      // Remove the phi instruction
      code_list.erase(iter);
    }
  }
}

}