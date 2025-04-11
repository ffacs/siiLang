#pragma once
#include "IR/IR.h"
#include "IR/value.h"

namespace SiiIR {

class CodeBuilder;
typedef std::shared_ptr< CodeBuilder > CodeBuilderPtr;

class CodeBuilder {
public:
  virtual ~CodeBuilder() {}
  virtual SiiIRBinaryOperationPtr append_multiply(ValuePtr left, ValuePtr right)
      = 0;
  virtual SiiIRBinaryOperationPtr append_divide(ValuePtr left, ValuePtr right)
      = 0;
  virtual SiiIRBinaryOperationPtr append_add(ValuePtr left, ValuePtr right) = 0;
  virtual SiiIRBinaryOperationPtr append_sub(ValuePtr left, ValuePtr right) = 0;
  virtual SiiIRUnaryOperationPtr  append_neg(ValuePtr child)                = 0;
  virtual SiiIRBinaryOperationPtr append_equal(ValuePtr left, ValuePtr right)
      = 0;
  virtual SiiIRBinaryOperationPtr append_not_equal(ValuePtr left,
                                                   ValuePtr right)
      = 0;
  virtual SiiIRBinaryOperationPtr append_less_than(ValuePtr left,
                                                   ValuePtr right)
      = 0;
  virtual SiiIRBinaryOperationPtr append_less_equal(ValuePtr left,
                                                    ValuePtr right)
      = 0;
  virtual SiiIRConditionBranchPtr append_condition_branch(ValuePtr expression,
                                                          LabelPtr true_label,
                                                          LabelPtr false_label)
      = 0;
  virtual SiiIRGotoPtr               append_goto(LabelPtr label)            = 0;
  virtual void                       append_label(LabelPtr label)           = 0;
  virtual SiiIRNopePtr               append_nope()                          = 0;
  virtual SiiIRFunctionDefinitionPtr append_function(FunctionValuePtr left) = 0;
  virtual SiiIRAllocaPtr append_alloca(uint32_t bytes, TypePtr type)        = 0;
  virtual SiiIRLoadPtr   append_load(ValuePtr source_address)               = 0;
  virtual SiiIRReturnPtr append_return(ValuePtr value)                      = 0;
  virtual SiiIRStorePtr  append_store(ValuePtr source, ValuePtr dest_address)
      = 0;
  virtual std::shared_ptr< std::vector< SiiIRCodePtr > > finish() = 0;
};

CodeBuilderPtr CreateCodeBuilder();
}  // namespace SiiIR
