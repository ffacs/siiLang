#pragma once
#include "IR/value.h"

namespace SiiIR {

class CodeBuilder;
typedef std::shared_ptr<CodeBuilder> CodeBuilderPtr;

class CodeBuilder {
public:
  virtual ~CodeBuilder() {}
  virtual void append_multiply(TemporaryValuePtr left,
                               TemporaryValuePtr right,
                               TemporaryValuePtr result) = 0;
  virtual void append_divide(TemporaryValuePtr left,
                             TemporaryValuePtr right,
                             TemporaryValuePtr result) = 0;
  virtual void append_add(TemporaryValuePtr left, TemporaryValuePtr right,
                          TemporaryValuePtr result) = 0;
  virtual void append_sub(TemporaryValuePtr left, TemporaryValuePtr right,
                          TemporaryValuePtr result) = 0;
  virtual void append_neg(TemporaryValuePtr child,
                          TemporaryValuePtr result) = 0;
  virtual void append_equal(TemporaryValuePtr left, TemporaryValuePtr right,
                            TemporaryValuePtr result) = 0;
  virtual void append_not_equal(TemporaryValuePtr left,
                                TemporaryValuePtr right,
                                TemporaryValuePtr result) = 0;
  virtual void append_less_than(TemporaryValuePtr left,
                                TemporaryValuePtr right,
                                TemporaryValuePtr result) = 0;
  virtual void append_less_equal(TemporaryValuePtr left,
                                 TemporaryValuePtr right,
                                 TemporaryValuePtr result) = 0;
  virtual void append_condition_branch(TemporaryValuePtr expression,
                                       LabelPtr true_label,
                                       LabelPtr false_label) = 0;
  virtual void append_goto(LabelPtr label) = 0;
  virtual std::pair<LabelFuturePtr, LabelFuturePtr>
  append_condition_branch(TemporaryValuePtr expression) = 0;
  virtual LabelFuturePtr append_goto() = 0;
  virtual void append_label(LabelPtr label) = 0;
  virtual void append_nope() = 0;
  virtual void append_function(FunctionValuePtr left) = 0;
  virtual void append_alloca(VariableValuePtr variable, uint32_t bytes) = 0;
  virtual void append_load(VariableValuePtr source,
                           TemporaryValuePtr dest) = 0;
  virtual VariableValuePtr append_store(TemporaryValuePtr source,
                                          VariableValuePtr dest) = 0;
  virtual VariableValuePtr append_store(VariableValuePtr source,
                                          VariableValuePtr dest) = 0;
  virtual std::shared_ptr<std::vector<SiiIRCodePtr>> finish() = 0;
};

CodeBuilderPtr CreateCodeBuilder();
} // namespace SiiIR
