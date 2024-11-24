#include "code_builder.h"

class CodeBuilderImpl : public CodeBuilder {
 public:
  AddressPtr append_multiply(AddressPtr left, AddressPtr right) override;
  AddressPtr append_divide(AddressPtr left, AddressPtr right) override;
  AddressPtr append_add(AddressPtr left, AddressPtr right) override;
  AddressPtr append_sub(AddressPtr left, AddressPtr right) override;
  AddressPtr append_neg(AddressPtr left) override;
  AddressPtr append_equal(AddressPtr left, AddressPtr right) override;
  AddressPtr append_not_equal(AddressPtr left, AddressPtr right) override;
  AddressPtr append_less_than(AddressPtr left, AddressPtr right) override;
  AddressPtr append_less_equal(AddressPtr left, AddressPtr right) override;
  AddressPtr append_assign(AddressPtr left, AddressPtr right) override;
  LabelPtr   new_label(const std::string& name = "") override;
  void       append_if_else(AddressPtr expression, LabelPtr true_label, LabelPtr false_label) override;
  void       append_goto(LabelPtr target_label) override;
  void       append_label(LabelPtr label) override;
  void       append_nope() override;
  std::vector<ThreeAddressCodePtr> finish() override;
 protected:
  void       append_new_tac(ThreeAddressCodePtr new_tac);
  std::vector<ThreeAddressCodePtr> code_list_;
  uint32_t temproray_variable_count_ = 0;
  uint32_t unnamed_label_count_ = 0;
  std::vector<LabelPtr> appended_labels_;
};

void CodeBuilderImpl::append_new_tac(ThreeAddressCodePtr new_tac) {
  if (!appended_labels_.empty()) {
    for (auto & label : appended_labels_) {
      label->dest_ = new_tac.get();
      new_tac->lable_list_.emplace_back(std::move(label));
    }
    appended_labels_.clear();
  } 
  code_list_.emplace_back(std::move(new_tac));
}

AddressPtr CodeBuilderImpl::append_multiply(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::MUL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_divide(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::DIV);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_add(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::ADD);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_sub(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::SUB);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_neg(AddressPtr child) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::NEG);
  new_tac->argL_ = std::move(child);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_equal(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_not_equal(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::NOT_EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_less_than(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::LESS_THAN);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_less_equal(AddressPtr left, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::LESS_EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  auto result = Address::temporary(new_tac.get(), std::to_string(temproray_variable_count_++));
  new_tac->result_ = result;
  append_new_tac(std::move(new_tac));
  return result;
}

AddressPtr CodeBuilderImpl::append_assign(AddressPtr result, AddressPtr right) {
  ThreeAddressCodePtr new_tac = std::make_shared<ThreeAddressCode>(TACOperator::ASSIGN);
  new_tac->result_ = result;
  new_tac->argL_ = std::move(right);
  append_new_tac(std::move(new_tac));
  return result;
}

LabelPtr CodeBuilderImpl::new_label(const std::string& name) {
  auto new_label_name = name;
  if (new_label_name.empty()) {
    new_label_name = std::to_string(unnamed_label_count_++);
  }
  return std::make_shared<Label>(nullptr, new_label_name);
}

void CodeBuilderImpl::append_if_else(AddressPtr expression, LabelPtr true_label, LabelPtr false_label) {
  ThreeAddressCodePtr if_true_goto = std::make_shared<ThreeAddressCode>(TACOperator::IF_TRUE_GOTO);
  if_true_goto->argL_ = expression;
  if_true_goto->jump_dest_ = std::move(true_label);
  append_new_tac(std::move(if_true_goto));
  ThreeAddressCodePtr then_goto = std::make_shared<ThreeAddressCode>(TACOperator::GOTO); 
  then_goto->jump_dest_ = std::move(false_label);
  append_new_tac(std::move(then_goto));
}

void CodeBuilderImpl::append_goto(LabelPtr target_label) {
  ThreeAddressCodePtr then_goto = std::make_shared<ThreeAddressCode>(TACOperator::GOTO); 
  then_goto->jump_dest_ = std::move(target_label);
  append_new_tac(std::move(then_goto));
}

void CodeBuilderImpl::append_label(LabelPtr label) {
  appended_labels_.emplace_back(std::move(label));
}

void CodeBuilderImpl::append_nope() {
  ThreeAddressCodePtr nope = std::make_shared<ThreeAddressCode>(TACOperator::NOPE); 
  append_new_tac(std::move(nope));
}

std::vector<ThreeAddressCodePtr> CodeBuilderImpl::finish() {
  if (!appended_labels_.empty()) {
    append_nope();
  }
  return std::move(code_list_);
}

std::unique_ptr<CodeBuilder> CreateCodeBuilder() {
  return std::make_unique<CodeBuilderImpl>();
}
