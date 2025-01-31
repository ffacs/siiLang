#include "IR/code_builder.h"
#include "IR/IR.h"

namespace SiiIR {
class CodeBuilderImpl : public CodeBuilder {
public:
  void append_multiply(TemporaryAddressPtr left, TemporaryAddressPtr right,
                       TemporaryAddressPtr result) override;
  void append_divide(TemporaryAddressPtr left, TemporaryAddressPtr right,
                     TemporaryAddressPtr result) override;
  void append_add(TemporaryAddressPtr left, TemporaryAddressPtr right,
                  TemporaryAddressPtr result) override;
  void append_sub(TemporaryAddressPtr left, TemporaryAddressPtr right,
                  TemporaryAddressPtr result) override;
  void append_neg(TemporaryAddressPtr left, TemporaryAddressPtr result) override;
  void append_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                    TemporaryAddressPtr result) override;
  void append_not_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                        TemporaryAddressPtr result) override;
  void append_less_than(TemporaryAddressPtr left, TemporaryAddressPtr right,
                        TemporaryAddressPtr result) override;
  void append_less_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                         TemporaryAddressPtr result) override;
  LabelPtr new_label(const std::string &name = "") override;
  void append_if_true_goto(TemporaryAddressPtr expression, LabelPtr label) override;
  void append_if_false_goto(TemporaryAddressPtr expression, LabelPtr label) override;
  void append_goto(LabelPtr label) override;
  LabelFuturePtr append_if_true_goto(TemporaryAddressPtr expression) override;
  LabelFuturePtr append_if_false_goto(TemporaryAddressPtr expression) override;
  LabelFuturePtr append_goto() override;
  void append_label(LabelPtr label) override;
  void append_nope() override;
  void append_function(FunctionAddressPtr func) override;
  void append_alloca(VariableAddressPtr variable, uint32_t bytes) override;
  void append_load(VariableAddressPtr source, TemporaryAddressPtr dest) override;
  VariableAddressPtr append_store(TemporaryAddressPtr source, VariableAddressPtr dest) override;
  std::shared_ptr<std::vector<SiiIRCodePtr>> finish() override;

protected:
  void append_new_code(SiiIRCodePtr new_code);
  std::vector<SiiIRCodePtr> alloca_list_;
  std::vector<SiiIRCodePtr> code_list_;
  uint32_t unnamed_label_count_ = 0;
  LabelPtr appended_label_;
};

void CodeBuilderImpl::append_new_code(SiiIRCodePtr new_code) {
  if (appended_label_ != nullptr) {
    appended_label_->dest_code_ = new_code.get();
    new_code->label_ = std::move(appended_label_);
    appended_label_.reset();
  }
  code_list_.emplace_back(std::move(new_code));
}

void CodeBuilderImpl::append_multiply(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                      TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::MUL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_divide(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                    TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::DIV, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_add(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                 TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::ADD, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_sub(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                 TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::SUB, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_neg(TemporaryAddressPtr child, TemporaryAddressPtr result) {
  SiiIRUnaryOperationPtr new_code = std::make_shared<SiiIRUnaryOperation>(
      SiiIRCodeKind::NEG, std::move(child), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                   TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_not_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                       TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::NOT_EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_less_than(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                       TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::LESS_THAN, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_less_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                        TemporaryAddressPtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::LESS_EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

VariableAddressPtr CodeBuilderImpl::append_store(TemporaryAddressPtr source, VariableAddressPtr dest) {
  SiiIRStorePtr new_code = std::make_shared<SiiIRStore>(std::move(source), dest);
  append_new_code(std::move(new_code));
  return dest;
}

void CodeBuilderImpl::append_load(VariableAddressPtr source, TemporaryAddressPtr dest) {
  SiiIRLoadPtr new_code = std::make_shared<SiiIRLoad>(std::move(source), dest);
  append_new_code(std::move(new_code));
}

LabelPtr CodeBuilderImpl::new_label(const std::string &name) {
  auto new_label_name = name;
  if (new_label_name.empty()) {
    new_label_name = std::to_string(unnamed_label_count_++);
  }
  return std::make_shared<Label>(nullptr, new_label_name);
}

void CodeBuilderImpl::append_if_true_goto(TemporaryAddressPtr expression, LabelPtr label) {
  SiiIRIfTrueGotoPtr if_true_goto = std::make_shared<SiiIRIfTrueGoto>(
      std::move(expression), std::move(label));
  append_new_code(std::move(if_true_goto));
}

void CodeBuilderImpl::append_if_false_goto(TemporaryAddressPtr expression, LabelPtr label) {
  SiiIRIfFalseGotoPtr if_false_goto = std::make_shared<SiiIRIfFalseGoto>(
      std::move(expression), std::move(label));
  append_new_code(std::move(if_false_goto));
}

void CodeBuilderImpl::append_goto(LabelPtr label) {
  SiiIRGotoPtr then_goto = std::make_shared<SiiIRGoto>(std::move(label));
  append_new_code(std::move(then_goto));
}

LabelFuturePtr CodeBuilderImpl::append_if_true_goto(TemporaryAddressPtr expression) {
  SiiIRIfTrueGotoPtr if_true_goto = std::make_shared<SiiIRIfTrueGoto>(
      std::move(expression), nullptr);
  auto labelFuture = std::make_shared<LabelFuture>(&(if_true_goto->dest_label_));
  append_new_code(std::move(if_true_goto));
  return labelFuture;
}

LabelFuturePtr CodeBuilderImpl::append_if_false_goto(TemporaryAddressPtr expression) {
  SiiIRIfFalseGotoPtr if_false_goto = std::make_shared<SiiIRIfFalseGoto>(
      std::move(expression), nullptr);
  auto labelFuture = std::make_shared<LabelFuture>(&(if_false_goto->dest_label_));
  append_new_code(std::move(if_false_goto));
  return labelFuture;
}

LabelFuturePtr CodeBuilderImpl::append_goto() {
  SiiIRGotoPtr then_goto = std::make_shared<SiiIRGoto>(nullptr);
  auto labelFuture = std::make_shared<LabelFuture>(&(then_goto->dest_label_));
  append_new_code(std::move(then_goto));
  return labelFuture;
}

void CodeBuilderImpl::append_label(LabelPtr label) {
  if (appended_label_ != nullptr) {
    auto future = append_goto();
    future->set_label(label);
    appended_label_ = nullptr;
  }
  appended_label_ = std::move(label);
}

void CodeBuilderImpl::append_nope() {
  SiiIRNopePtr nope = std::make_shared<SiiIRNope>();
  append_new_code(std::move(nope));
}

std::shared_ptr<std::vector<SiiIRCodePtr>> CodeBuilderImpl::finish() {
  if (appended_label_ != nullptr) {
    append_nope();
  }
  std::shared_ptr<std::vector<SiiIRCodePtr>> result =
      std::make_shared<std::vector<SiiIRCodePtr>>();
  result->insert(result->end(), alloca_list_.begin(), alloca_list_.end());
  result->insert(result->end(), code_list_.begin(), code_list_.end());
  return result;
}

void CodeBuilderImpl::append_function(FunctionAddressPtr func) {
  SiiIRFunctionDefinitionPtr function =
      std::make_shared<SiiIRFunctionDefinition>(std::move(func));
  append_new_code(std::move(function));
}

void CodeBuilderImpl::append_alloca(VariableAddressPtr variable, uint32_t bytes) {
  SiiIRAllocaPtr alloca =
      std::make_shared<SiiIRAlloca>(std::move(variable), bytes);
  alloca_list_.push_back(alloca);
}

CodeBuilderPtr CreateCodeBuilder() {
  return std::make_shared<CodeBuilderImpl>();
}

} // namespace SiiIR