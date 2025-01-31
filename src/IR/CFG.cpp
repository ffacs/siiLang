#include "IR/CFG.h"

#include <map>

namespace SiiIR {

class CFGBuilder {
private:
  std::shared_ptr<std::vector<SiiIRCodePtr>> codes_;
  std::map<std::string, BasicGroupNodePtr> label_to_node_;
  std::vector<BasicGroupNodePtr> basic_groups_;

  BasicGroupNode *build_basic_group_starting_from(SiiIRCode *start) {
    BasicGroupNodePtr result = nullptr;
    if (start->label_ != nullptr) {
      if (label_to_node_.find(start->label_->name_) != label_to_node_.end()) {
        return label_to_node_[start->label_->name_].get();
      }
      result = std::make_shared<BasicGroupNode>();
      label_to_node_[start->label_->name_] = result;
      // make sure we won't stuck in a loop
      result->basic_group_->codes_.push_back(start);
      start = start->next_;
    } else {
      result = std::make_shared<BasicGroupNode>();
    }

    std::vector<SiiIRCode *> &target_codes = result->basic_group_->codes_;
    SiiIRCode *current = start;
    while (current != nullptr) {
      if (current->label_ != nullptr) {
        BasicGroupNode *next_group = build_basic_group_starting_from(current);
        result->follows_.push_back(next_group);
        break;
      }
      target_codes.push_back(current);
      if (current->kind_ == SiiIRCodeKind::GOTO) {
        BasicGroupNode *next_group = build_basic_group_starting_from(
            static_cast<SiiIRGoto *>(current)->dest_label_->dest_code_);
        result->follows_.push_back(next_group);
        break;
      }
      if (current->kind_ == SiiIRCodeKind::CONDITION_BRANCH) {
        BasicGroupNode *true_group = build_basic_group_starting_from(
            static_cast<SiiIRConditionBarnch *>(current)
                ->true_label_->dest_code_);
        result->follows_.push_back(true_group);
        true_group->precedes_.push_back(result.get());

        BasicGroupNode *false_group = build_basic_group_starting_from(
            static_cast<SiiIRConditionBarnch *>(current)
                ->false_label_->dest_code_);
        result->follows_.push_back(false_group);
        false_group->precedes_.push_back(result.get());
        break;
      }
      current = current->next_;
    }

    basic_groups_.push_back(result);
    return result.get();
  }

public:
  explicit CFGBuilder(std::shared_ptr<std::vector<SiiIRCodePtr>> codes)
      : codes_(std::move(codes)) {}

  CFGPtr build() {
    for (size_t i = 0; i < codes_->size(); ++i) {
      if (i + 1 < codes_->size()) {
        codes_->at(i)->next_ = codes_->at(i + 1).get();
      } else {
        codes_->at(i)->next_ = nullptr;
      }
    }

    CFGPtr result_cfg = std::make_shared<CFG>();
    // Create an entry node that contains no code.
    std::shared_ptr<BasicGroupNode> entry = std::make_shared<BasicGroupNode>();
    basic_groups_.push_back(entry);
    result_cfg->entry_ = entry.get();

    if (!codes_->empty()) {
      auto follow = build_basic_group_starting_from(codes_->front().get());
      entry->follows_.push_back(follow);
      follow->precedes_.push_back(entry.get());
    }
    result_cfg->basic_groups_ = std::move(basic_groups_);
    result_cfg->codes_in_order_ = std::move(codes_);
    return result_cfg;
  }
};

CFGPtr BuildCFG(std::shared_ptr<std::vector<SiiIRCodePtr>> codes) {
  CFGBuilder builder(std::move(codes));
  return builder.build();
}

} // namespace SiiIR