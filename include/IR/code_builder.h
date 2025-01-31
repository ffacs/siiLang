#pragma once
#include "IR/address.h"

namespace SiiIR {

class CodeBuilder;
typedef std::shared_ptr<CodeBuilder> CodeBuilderPtr;

class CodeBuilder {
public:
  virtual ~CodeBuilder() {}
  virtual void append_multiply(TemporaryAddressPtr left, TemporaryAddressPtr right,
                               TemporaryAddressPtr result) = 0;
  virtual void append_divide(TemporaryAddressPtr left, TemporaryAddressPtr right,
                             TemporaryAddressPtr result) = 0;
  virtual void append_add(TemporaryAddressPtr left, TemporaryAddressPtr right,
                          TemporaryAddressPtr result) = 0;
  virtual void append_sub(TemporaryAddressPtr left, TemporaryAddressPtr right,
                          TemporaryAddressPtr result) = 0;
  virtual void append_neg(TemporaryAddressPtr child, TemporaryAddressPtr result) = 0;
  virtual void append_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                            TemporaryAddressPtr result) = 0;
  virtual void append_not_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                TemporaryAddressPtr result) = 0;
  virtual void append_less_than(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                TemporaryAddressPtr result) = 0;
  virtual void append_less_equal(TemporaryAddressPtr left, TemporaryAddressPtr right,
                                 TemporaryAddressPtr result) = 0;
  virtual void append_if_false_goto(TemporaryAddressPtr expression, LabelPtr label) = 0;
  virtual void append_if_true_goto(TemporaryAddressPtr expression, LabelPtr label) = 0;
  virtual void append_goto(LabelPtr label) = 0;
  virtual LabelFuturePtr append_if_true_goto(TemporaryAddressPtr expression) = 0;
  virtual LabelFuturePtr append_if_false_goto(TemporaryAddressPtr expression) = 0;
  virtual LabelFuturePtr append_goto() = 0;
  virtual void append_label(LabelPtr label) = 0;
  virtual void append_nope() = 0;
  virtual void append_function(FunctionAddressPtr left) = 0;
  virtual void append_alloca(VariableAddressPtr variable, uint32_t bytes) = 0;
  virtual void append_load(VariableAddressPtr source, TemporaryAddressPtr dest) = 0;
  virtual VariableAddressPtr append_store(TemporaryAddressPtr source, VariableAddressPtr dest) = 0;
  virtual std::shared_ptr<std::vector<SiiIRCodePtr>> finish() = 0;
};

CodeBuilderPtr CreateCodeBuilder();
} // namespace front
