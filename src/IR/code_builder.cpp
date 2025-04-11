#include "IR/code_builder.h"
#include "IR/IR.h"
#include <stdexcept>

namespace SiiIR {
class CodeBuilderImpl : public CodeBuilder {
public:
  SiiIRBinaryOperationPtr append_multiply(ValuePtr left,
                                          ValuePtr right) override;
  SiiIRBinaryOperationPtr append_divide(ValuePtr left, ValuePtr right) override;
  SiiIRBinaryOperationPtr append_add(ValuePtr left, ValuePtr right) override;
  SiiIRBinaryOperationPtr append_sub(ValuePtr left, ValuePtr right) override;
  SiiIRUnaryOperationPtr  append_neg(ValuePtr left) override;
  SiiIRBinaryOperationPtr append_equal(ValuePtr left, ValuePtr right) override;
  SiiIRBinaryOperationPtr append_not_equal(ValuePtr left,
                                           ValuePtr right) override;
  SiiIRBinaryOperationPtr append_less_than(ValuePtr left,
                                           ValuePtr right) override;
  SiiIRBinaryOperationPtr append_less_equal(ValuePtr left,
                                            ValuePtr right) override;
  SiiIRConditionBranchPtr
                             append_condition_branch(ValuePtr expression,
                                                     LabelPtr true_label,
                                                     LabelPtr false_label) override;
  SiiIRGotoPtr               append_goto(LabelPtr label) override;
  void                       append_label(LabelPtr label) override;
  SiiIRNopePtr               append_nope() override;
  SiiIRFunctionDefinitionPtr append_function(FunctionValuePtr func) override;
  SiiIRAllocaPtr append_alloca(uint32_t bytes, TypePtr type) override;
  SiiIRLoadPtr   append_load(ValuePtr source_address) override;
  SiiIRReturnPtr append_return(ValuePtr value) override;
  SiiIRStorePtr  append_store(ValuePtr source, ValuePtr dest_address) override;
  std::shared_ptr< std::vector< SiiIRCodePtr > > finish() override;

protected:
  void                        append_new_code(SiiIRCodePtr new_code);
  std::vector< SiiIRCodePtr > alloca_list_;
  std::vector< SiiIRCodePtr > code_list_;
  uint32_t                    unnamed_label_count_ = 0;
  LabelPtr                    appended_label_;
};

void CodeBuilderImpl::append_new_code(SiiIRCodePtr new_code) {
  if ( appended_label_ != nullptr ) {
    appended_label_->dest_code_ = new_code.get();
    new_code->label_            = std::move(appended_label_);
    appended_label_.reset();
  }
  code_list_.emplace_back(std::move(new_code));
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_multiply(ValuePtr left,
                                                         ValuePtr right) {
  SiiIRBinaryOperationPtr new_code = std::make_shared< SiiIRBinaryOperation >(
      SiiIRCodeKind::MUL, std::move(left), std::move(right), left->type_);
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_divide(ValuePtr left,
                                                       ValuePtr right) {
  SiiIRBinaryOperationPtr new_code = std::make_shared< SiiIRBinaryOperation >(
      SiiIRCodeKind::DIV, std::move(left), std::move(right), left->type_);
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_add(ValuePtr left,
                                                    ValuePtr right) {
  SiiIRBinaryOperationPtr new_code = std::make_shared< SiiIRBinaryOperation >(
      SiiIRCodeKind::ADD, std::move(left), std::move(right), left->type_);
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_sub(ValuePtr left,
                                                    ValuePtr right) {
  SiiIRBinaryOperationPtr new_code = std::make_shared< SiiIRBinaryOperation >(
      SiiIRCodeKind::SUB, std::move(left), std::move(right), left->type_);
  append_new_code(new_code);
  return new_code;
}

SiiIRUnaryOperationPtr CodeBuilderImpl::append_neg(ValuePtr child) {
  SiiIRUnaryOperationPtr new_code = std::make_shared< SiiIRUnaryOperation >(
      SiiIRCodeKind::NEG, std::move(child));
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_equal(ValuePtr left,
                                                      ValuePtr right) {
  SiiIRBinaryOperationPtr new_code
      = std::make_shared< SiiIRBinaryOperation >(SiiIRCodeKind::EQUAL,
                                                 std::move(left),
                                                 std::move(right),
                                                 Type::Integer(1));
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_not_equal(ValuePtr left,
                                                          ValuePtr right) {
  if ( *left->type_ != *right->type_ ) {
    throw std::runtime_error("Not equal must be of same type");
  }
  SiiIRBinaryOperationPtr new_code
      = std::make_shared< SiiIRBinaryOperation >(SiiIRCodeKind::NOT_EQUAL,
                                                 std::move(left),
                                                 std::move(right),
                                                 Type::Integer(1));
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_less_than(ValuePtr left,
                                                          ValuePtr right) {
  if ( *left->type_ != *right->type_ ) {
    throw std::runtime_error("Less than must be of same type");
  }
  SiiIRBinaryOperationPtr new_code
      = std::make_shared< SiiIRBinaryOperation >(SiiIRCodeKind::LESS_THAN,
                                                 std::move(left),
                                                 std::move(right),
                                                 Type::Integer(1));
  append_new_code(new_code);
  return new_code;
}

SiiIRBinaryOperationPtr CodeBuilderImpl::append_less_equal(ValuePtr left,
                                                           ValuePtr right) {
  if ( *left->type_ != *right->type_ ) {
    throw std::runtime_error("Less equal must be of same type");
  }
  SiiIRBinaryOperationPtr new_code
      = std::make_shared< SiiIRBinaryOperation >(SiiIRCodeKind::LESS_EQUAL,
                                                 std::move(left),
                                                 std::move(right),
                                                 Type::Integer(1));
  append_new_code(new_code);
  return new_code;
}

SiiIRStorePtr CodeBuilderImpl::append_store(ValuePtr source,
                                            ValuePtr dest_address) {
  auto aim_type = Type::GetAimType(dest_address->type_);
  if ( *aim_type != *source->type_ ) {
    throw std::runtime_error("Store must be of same type");
  }
  SiiIRStorePtr new_code = std::make_shared< SiiIRStore >(source, dest_address);
  append_new_code(new_code);
  return new_code;
}

SiiIRLoadPtr CodeBuilderImpl::append_load(ValuePtr source_address) {
  SiiIRLoadPtr new_code
      = std::make_shared< SiiIRLoad >(std::move(source_address));
  append_new_code(new_code);
  return new_code;
}

SiiIRReturnPtr CodeBuilderImpl::append_return(ValuePtr value) {
  SiiIRReturnPtr new_code = std::make_shared< SiiIRReturn >(std::move(value));
  append_new_code(new_code);
  return new_code;
}

SiiIRConditionBranchPtr
CodeBuilderImpl::append_condition_branch(ValuePtr condition,
                                         LabelPtr true_label,
                                         LabelPtr false_label) {
  if ( *condition->type_ != *Type::Integer(1) ) {
    throw std::runtime_error("Condition branch must be of type bool");
  }
  SiiIRConditionBranchPtr new_code = std::make_shared< SiiIRConditionBranch >(
      std::move(condition), std::move(true_label), std::move(false_label));
  append_new_code(new_code);
  return new_code;
}

SiiIRGotoPtr CodeBuilderImpl::append_goto(LabelPtr label) {
  SiiIRGotoPtr then_goto = std::make_shared< SiiIRGoto >(std::move(label));
  append_new_code(then_goto);
  return then_goto;
}

void CodeBuilderImpl::append_label(LabelPtr label) {
  if ( appended_label_ != nullptr ) {
    append_goto(label);
    appended_label_ = nullptr;
  }
  appended_label_ = std::move(label);
}

SiiIRNopePtr CodeBuilderImpl::append_nope() {
  SiiIRNopePtr nope = std::make_shared< SiiIRNope >();
  append_new_code(nope);
  return nope;
}

std::shared_ptr< std::vector< SiiIRCodePtr > > CodeBuilderImpl::finish() {
  if ( appended_label_ != nullptr ) {
    append_nope();
  }
  std::shared_ptr< std::vector< SiiIRCodePtr > > result
      = std::make_shared< std::vector< SiiIRCodePtr > >();
  result->insert(result->end(), alloca_list_.begin(), alloca_list_.end());
  result->insert(result->end(), code_list_.begin(), code_list_.end());
  return result;
}

SiiIRFunctionDefinitionPtr
CodeBuilderImpl::append_function(FunctionValuePtr func) {
  SiiIRFunctionDefinitionPtr function
      = std::make_shared< SiiIRFunctionDefinition >(std::move(func));
  append_new_code(function);
  return function;
}

SiiIRAllocaPtr CodeBuilderImpl::append_alloca(uint32_t bytes, TypePtr type) {
  SiiIRAllocaPtr alloca = std::make_shared< SiiIRAlloca >(bytes, type);
  alloca_list_.push_back(alloca);
  return alloca;
}

CodeBuilderPtr CreateCodeBuilder() {
  return std::make_shared< CodeBuilderImpl >();
}

}  // namespace SiiIR
