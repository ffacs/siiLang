#include "front/code_builder.h"
#include "IR/IR.h"

using namespace SiiIR;

namespace front {
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
  AddressPtr append_assign(AddressPtr dest, AddressPtr src) override;
  LabelPtr new_label(const std::string &name = "") override;
  void append_if_true_goto(AddressPtr expression,
                           LabelPtr target_label) override;
  void append_if_false_goto(AddressPtr expression,
                            LabelPtr target_label) override;
  void append_goto(LabelPtr target_label) override;
  void append_label(LabelPtr label) override;
  void append_nope() override;
  void append_function(FunctionAddressPtr func) override;
  void append_alloca(AddressPtr variable, uint32_t bytes) override;
  std::vector<SiiIRCodePtr> finish() override;

protected:
  void append_new_code(SiiIRCodePtr new_code);
  std::vector<SiiIRCodePtr> alloca_list_;
  std::vector<SiiIRCodePtr> code_list_;
  uint32_t unnamed_label_count_ = 0;
  std::vector<LabelPtr> appended_labels_;
};

void CodeBuilderImpl::append_new_code(SiiIRCodePtr new_code) {
  if (!appended_labels_.empty()) {
    for (auto &label : appended_labels_) {
      label->dest_ = new_code.get();
      new_code->labels_.emplace_back(std::move(label));
    }
    appended_labels_.clear();
  }
  code_list_.emplace_back(std::move(new_code));
}

void CodeBuilderImpl::append_multiply(AddressPtr left, AddressPtr right,
                                      TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::MUL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_divide(AddressPtr left, AddressPtr right,
                                    TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::DIV, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_add(AddressPtr left, AddressPtr right,
                                 TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::ADD, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_sub(AddressPtr left, AddressPtr right,
                                 TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::SUB, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_neg(AddressPtr child, TemporaryAddressPtr result) {
  SiiIRUnaryOperationPtr new_code = std::make_shared<SiiIRUnaryOperation>(
      SiiIRCodeKind::NEG, std::move(child), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_equal(AddressPtr left, AddressPtr right,
                                   TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_not_equal(AddressPtr left, AddressPtr right,
                                       TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::NOT_EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_less_than(AddressPtr left, AddressPtr right,
                                       TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::LESS_THAN, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_less_equal(AddressPtr left, AddressPtr right,
                                        TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::LESS_EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

AddressPtr CodeBuilderImpl::append_assign(AddressPtr dest, AddressPtr src) {
  SiiIRAssignPtr new_code = std::make_shared<SiiIRAssign>(std::move(src), dest);
  append_new_code(std::move(new_code));
  return dest;
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
  SiiIRIfTrueGotoPtr if_true_goto = std::make_shared<SiiIRIfTrueGoto>(
      std::move(expression), std::move(target_label));
  append_new_code(std::move(if_true_goto));
}

void CodeBuilderImpl::append_if_false_goto(AddressPtr expression,
                                           LabelPtr target_label) {
  SiiIRIfFalseGotoPtr if_false_goto = std::make_shared<SiiIRIfFalseGoto>(
      std::move(expression), std::move(target_label));
  append_new_code(std::move(if_false_goto));
}

void CodeBuilderImpl::append_goto(LabelPtr target_label) {
  SiiIRGotoPtr then_goto = std::make_shared<SiiIRGoto>(std::move(target_label));
  append_new_code(std::move(then_goto));
}

void CodeBuilderImpl::append_label(LabelPtr label) {
  appended_labels_.emplace_back(std::move(label));
}

void CodeBuilderImpl::append_nope() {
  SiiIRNopePtr nope = std::make_shared<SiiIRNope>();
  append_new_code(std::move(nope));
}

std::vector<SiiIRCodePtr> CodeBuilderImpl::finish() {
  if (!appended_labels_.empty()) {
    append_nope();
  }
  std::vector<SiiIRCodePtr> result;
  result.insert(result.end(), alloca_list_.begin(), alloca_list_.end());
  result.insert(result.end(), code_list_.begin(), code_list_.end());
  return result;
}

void CodeBuilderImpl::append_function(FunctionAddressPtr func) {
  SiiIRFunctionDefinitionPtr function =
      std::make_shared<SiiIRFunctionDefinition>(std::move(func));
  append_new_code(std::move(function));
}

void CodeBuilderImpl::append_alloca(AddressPtr variable, uint32_t bytes) {
  SiiIRAllocaPtr alloca =
      std::make_shared<SiiIRAlloca>(std::move(variable), bytes);
  alloca_list_.push_back(alloca);
}

CodeBuilderPtr CreateCodeBuilder() {
  return std::make_shared<CodeBuilderImpl>();
}

} // namespace front