#include "front/code_builder.h"

class CodeBuilderImpl : public CodeBuilder {
public:
  void append_multiply(AddressPtr left, AddressPtr right,
                       TemporaryAddressPtr result) override;
  void append_divide(AddressPtr left, AddressPtr right,
                     TemporaryAddressPtr result) override;
  void append_add(AddressPtr left, AddressPtr right,
                  TemporaryAddressPtr result) override;
  void append_sub(AddressPtr left, AddressPtr right,
                  TemporaryAddressPtr result) override;
  void append_neg(AddressPtr left, TemporaryAddressPtr result) override;
  void append_equal(AddressPtr left, AddressPtr right,
                    TemporaryAddressPtr result) override;
  void append_not_equal(AddressPtr left, AddressPtr right,
                        TemporaryAddressPtr result) override;
  void append_less_than(AddressPtr left, AddressPtr right,
                        TemporaryAddressPtr result) override;
  void append_less_equal(AddressPtr left, AddressPtr right,
                         TemporaryAddressPtr result) override;
  AddressPtr append_assign(AddressPtr result, AddressPtr right) override;
  LabelPtr new_label(const std::string &name = "") override;
  void append_if_true_goto(AddressPtr expression,
                           LabelPtr target_label) override;
  void append_if_false_goto(AddressPtr expression,
                            LabelPtr target_label) override;
  void append_goto(LabelPtr target_label) override;
  void append_label(LabelPtr label) override;
  void append_nope() override;
  void append_function(FunctionAddressPtr left) override;
  void append_alloca(AddressPtr variable, uint32_t bytes) override;
  std::vector<ThreeAddressCodePtr> finish() override;

protected:
  void append_new_tac(ThreeAddressCodePtr new_tac);
  std::vector<ThreeAddressCodePtr> alloca_list_;
  std::vector<ThreeAddressCodePtr> code_list_;
  uint32_t unnamed_label_count_ = 0;
  std::vector<LabelPtr> appended_labels_;
};

void CodeBuilderImpl::append_new_tac(ThreeAddressCodePtr new_tac) {
  if (!appended_labels_.empty()) {
    for (auto &label : appended_labels_) {
      label->dest_ = new_tac.get();
      new_tac->lable_list_.emplace_back(std::move(label));
    }
    appended_labels_.clear();
  }
  code_list_.emplace_back(std::move(new_tac));
}

void CodeBuilderImpl::append_multiply(AddressPtr left, AddressPtr right,
                                      TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::MUL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_divide(AddressPtr left, AddressPtr right,
                                    TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::DIV);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_add(AddressPtr left, AddressPtr right,
                                 TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::ADD);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_sub(AddressPtr left, AddressPtr right,
                                 TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::SUB);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_neg(AddressPtr child, TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::NEG);
  new_tac->argL_ = std::move(child);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_equal(AddressPtr left, AddressPtr right,
                                   TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_not_equal(AddressPtr left, AddressPtr right,
                                       TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::NOT_EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_less_than(AddressPtr left, AddressPtr right,
                                       TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::LESS_THAN);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

void CodeBuilderImpl::append_less_equal(AddressPtr left, AddressPtr right,
                                        TemporaryAddressPtr result) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::LESS_EQUAL);
  new_tac->argL_ = std::move(left);
  new_tac->argR_ = std::move(right);
  new_tac->result_ = result;
  result->src_ = new_tac.get();
  append_new_tac(std::move(new_tac));
}

AddressPtr CodeBuilderImpl::append_assign(AddressPtr result, AddressPtr right) {
  ThreeAddressCodePtr new_tac =
      std::make_shared<ThreeAddressCode>(TACOperator::ASSIGN);
  new_tac->result_ = result;
  new_tac->argL_ = std::move(right);
  append_new_tac(std::move(new_tac));
  return result;
}

LabelPtr CodeBuilderImpl::new_label(const std::string &name) {
  auto new_label_name = name;
  if (new_label_name.empty()) {
    new_label_name = std::to_string(unnamed_label_count_++);
  }
  return std::make_shared<Label>(nullptr, new_label_name);
}

void CodeBuilderImpl::append_if_true_goto(AddressPtr expression,
                                          LabelPtr target_label) {
  ThreeAddressCodePtr if_true_goto =
      std::make_shared<ThreeAddressCode>(TACOperator::IF_TRUE_GOTO);
  if_true_goto->argL_ = expression;
  if_true_goto->jump_dest_ = std::move(target_label);
  append_new_tac(std::move(if_true_goto));
}

void CodeBuilderImpl::append_if_false_goto(AddressPtr expression,
                                           LabelPtr target_label) {
  ThreeAddressCodePtr if_false_goto =
      std::make_shared<ThreeAddressCode>(TACOperator::IF_FALSE_GOTO);
  if_false_goto->argL_ = expression;
  if_false_goto->jump_dest_ = std::move(target_label);
  append_new_tac(std::move(if_false_goto));
}

void CodeBuilderImpl::append_goto(LabelPtr target_label) {
  ThreeAddressCodePtr then_goto =
      std::make_shared<ThreeAddressCode>(TACOperator::GOTO);
  then_goto->jump_dest_ = std::move(target_label);
  append_new_tac(std::move(then_goto));
}

void CodeBuilderImpl::append_label(LabelPtr label) {
  appended_labels_.emplace_back(std::move(label));
}

void CodeBuilderImpl::append_nope() {
  ThreeAddressCodePtr nope =
      std::make_shared<ThreeAddressCode>(TACOperator::NOPE);
  append_new_tac(std::move(nope));
}

std::vector<ThreeAddressCodePtr> CodeBuilderImpl::finish() {
  if (!appended_labels_.empty()) {
    append_nope();
  }
  std::vector<ThreeAddressCodePtr> result;
  result.insert(result.end(), alloca_list_.begin(), alloca_list_.end());
  result.insert(result.end(), code_list_.begin(), code_list_.end());
  return result;
}

void CodeBuilderImpl::append_function(FunctionAddressPtr left) {
  ThreeAddressCodePtr function =
      std::make_shared<ThreeAddressCode>(TACOperator::FUNCTION_DEFINITION);
  function->argL_ = std::move(left);
  append_new_tac(std::move(function));
}

void CodeBuilderImpl::append_alloca(AddressPtr variable, uint32_t bytes) {
  ThreeAddressCodePtr alloca =
      std::make_shared<ThreeAddressCode>(TACOperator::ALLOCA);
  alloca->argL_ = std::move(variable);
  alloca->alloca_bytes_ = bytes;
  alloca_list_.push_back(alloca);
}

CodeBuilderPtr CreateCodeBuilder() {
  return std::make_shared<CodeBuilderImpl>();
}
