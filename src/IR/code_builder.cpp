#include "IR/code_builder.h"
#include "IR/IR.h"

namespace SiiIR {
class CodeBuilderImpl : public CodeBuilder {
public:
  void append_multiply(TemporaryValuePtr left, TemporaryValuePtr right,
                       TemporaryValuePtr result) override;
  void append_divide(TemporaryValuePtr left, TemporaryValuePtr right,
                     TemporaryValuePtr result) override;
  void append_add(TemporaryValuePtr left, TemporaryValuePtr right,
                  TemporaryValuePtr result) override;
  void append_sub(TemporaryValuePtr left, TemporaryValuePtr right,
                  TemporaryValuePtr result) override;
  void append_neg(TemporaryValuePtr left,
                  TemporaryValuePtr result) override;
  void append_equal(TemporaryValuePtr left, TemporaryValuePtr right,
                    TemporaryValuePtr result) override;
  void append_not_equal(TemporaryValuePtr left, TemporaryValuePtr right,
                        TemporaryValuePtr result) override;
  void append_less_than(TemporaryValuePtr left, TemporaryValuePtr right,
                        TemporaryValuePtr result) override;
  void append_less_equal(TemporaryValuePtr left, TemporaryValuePtr right,
                         TemporaryValuePtr result) override;
  void append_condition_branch(TemporaryValuePtr expression,
                               LabelPtr true_label,
                               LabelPtr false_label) override;
  void append_goto(LabelPtr label) override;
  std::pair<LabelFuturePtr, LabelFuturePtr>
  append_condition_branch(TemporaryValuePtr expression) override;
  LabelFuturePtr append_goto() override;
  void append_label(LabelPtr label) override;
  void append_nope() override;
  void append_function(FunctionValuePtr func) override;
  void append_alloca(VariableValuePtr variable, uint32_t bytes) override;
  void append_load(VariableValuePtr source,
                   TemporaryValuePtr dest) override;
  VariableValuePtr append_store(TemporaryValuePtr source,
                                  VariableValuePtr dest) override;
  VariableValuePtr append_store(VariableValuePtr source,
                                  VariableValuePtr dest) override;
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

void CodeBuilderImpl::append_multiply(TemporaryValuePtr left,
                                      TemporaryValuePtr right,
                                      TemporaryValuePtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::MUL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_divide(TemporaryValuePtr left,
                                    TemporaryValuePtr right,
                                    TemporaryValuePtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::DIV, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_add(TemporaryValuePtr left,
                                 TemporaryValuePtr right,
                                 TemporaryValuePtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::ADD, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_sub(TemporaryValuePtr left,
                                 TemporaryValuePtr right,
                                 TemporaryValuePtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::SUB, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_neg(TemporaryValuePtr child,
                                 TemporaryValuePtr result) {
  SiiIRUnaryOperationPtr new_code = std::make_shared<SiiIRUnaryOperation>(
      SiiIRCodeKind::NEG, std::move(child), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_equal(TemporaryValuePtr left,
                                   TemporaryValuePtr right,
                                   TemporaryValuePtr result) {
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_not_equal(TemporaryValuePtr left,
                                       TemporaryValuePtr right,
                                       TemporaryValuePtr result) {
  if (*left->type_ != *right->type_) {
    throw std::runtime_error("Not equal must be of same type");
  }
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::NOT_EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_less_than(TemporaryValuePtr left,
                                       TemporaryValuePtr right,
                                       TemporaryValuePtr result) {
  if (*left->type_ != *right->type_) {
    throw std::runtime_error("Less than must be of same type");
  }
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::LESS_THAN, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_less_equal(TemporaryValuePtr left,
                                        TemporaryValuePtr right,
                                        TemporaryValuePtr result) {
  if (*left->type_ != *right->type_) {
    throw std::runtime_error("Less equal must be of same type");
  }
  SiiIRBinaryOperationPtr new_code = std::make_shared<SiiIRBinaryOperation>(
      SiiIRCodeKind::LESS_EQUAL, std::move(left), std::move(right), result);
  result->src_ = new_code.get();
  append_new_code(std::move(new_code));
}

VariableValuePtr CodeBuilderImpl::append_store(TemporaryValuePtr source,
                                                 VariableValuePtr dest) {
  if (*dest->type_ != *source->type_) {
    throw std::runtime_error("Store must be of same type");
  }
  SiiIRStorePtr new_code =
      std::make_shared<SiiIRStore>(std::move(source), dest);
  append_new_code(std::move(new_code));
  return dest;
}

VariableValuePtr CodeBuilderImpl::append_store(VariableValuePtr source,
                                                 VariableValuePtr dest) {
  if (*source->type_ != *dest->type_) {
    throw std::runtime_error("Store must be of same type");
  }
  SiiIRStorePtr new_code =
      std::make_shared<SiiIRStore>(std::move(source), dest);
  append_new_code(std::move(new_code));
  return dest;
}

void CodeBuilderImpl::append_load(VariableValuePtr source,
                                  TemporaryValuePtr dest) {
  SiiIRLoadPtr new_code = std::make_shared<SiiIRLoad>(std::move(source), std::move(dest));
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_condition_branch(TemporaryValuePtr condition,
                                              LabelPtr true_label,
                                              LabelPtr false_label) {
  if (*condition->type_ != *Type::Integer(1)) {
      throw std::runtime_error("Condition branch must be of type bool");
  }
  SiiIRConditionBarnchPtr new_code = std::make_shared<SiiIRConditionBarnch>(
      std::move(condition), std::move(true_label), std::move(false_label));
  append_new_code(std::move(new_code));
}

void CodeBuilderImpl::append_goto(LabelPtr label) {
  SiiIRGotoPtr then_goto = std::make_shared<SiiIRGoto>(std::move(label));
  append_new_code(std::move(then_goto));
}

std::pair<LabelFuturePtr, LabelFuturePtr>
CodeBuilderImpl::append_condition_branch(TemporaryValuePtr condition) {
  if (*condition->type_ != *Type::Integer(1)) {
    throw std::runtime_error("Condition branch must be of type bool");
  }
  SiiIRConditionBarnchPtr new_code = std::make_shared<SiiIRConditionBarnch>(
      std::move(condition), nullptr, nullptr);
  auto true_label_future =
      std::make_shared<LabelFuture>(&(new_code->true_label_));
  auto false_label_future =
      std::make_shared<LabelFuture>(&(new_code->false_label_));
  append_new_code(std::move(new_code));
  return std::make_pair(true_label_future, false_label_future);
}

LabelFuturePtr CodeBuilderImpl::append_goto() {
  SiiIRGotoPtr then_goto = std::make_shared<SiiIRGoto>(nullptr);
  auto labelFuture = std::make_shared<LabelFuture>(&(then_goto->dest_label_));
  append_new_code(std::move(then_goto));
  return labelFuture;
}

void CodeBuilderImpl::append_label(LabelPtr label) {
  if (appended_label_ != nullptr) {
    append_goto(label);
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

void CodeBuilderImpl::append_function(FunctionValuePtr func) {
  SiiIRFunctionDefinitionPtr function =
      std::make_shared<SiiIRFunctionDefinition>(std::move(func));
  append_new_code(std::move(function));
}

void CodeBuilderImpl::append_alloca(VariableValuePtr variable,
                                    uint32_t bytes) {
  SiiIRAllocaPtr alloca =
      std::make_shared<SiiIRAlloca>(std::move(variable), bytes);
  alloca_list_.push_back(alloca);
}

CodeBuilderPtr CreateCodeBuilder() {
  return std::make_shared<CodeBuilderImpl>();
}

} // namespace SiiIR