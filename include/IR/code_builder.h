#pragma once
#include "IR/address.h"

namespace SiiIR {

class CodeBuilder;
typedef std::shared_ptr<CodeBuilder> CodeBuilderPtr;

class CodeBuilder {
public:
  virtual ~CodeBuilder() {}
  virtual void append_multiply(AddressPtr left, AddressPtr right,
                               TemporaryAddressPtr result) = 0;
  virtual void append_divide(AddressPtr left, AddressPtr right,
                             TemporaryAddressPtr result) = 0;
  virtual void append_add(AddressPtr left, AddressPtr right,
                          TemporaryAddressPtr result) = 0;
  virtual void append_sub(AddressPtr left, AddressPtr right,
                          TemporaryAddressPtr result) = 0;
  virtual void append_neg(AddressPtr child, TemporaryAddressPtr result) = 0;
  virtual void append_equal(AddressPtr left, AddressPtr right,
                            TemporaryAddressPtr result) = 0;
  virtual void append_not_equal(AddressPtr left, AddressPtr right,
                                TemporaryAddressPtr result) = 0;
  virtual void append_less_than(AddressPtr left, AddressPtr right,
                                TemporaryAddressPtr result) = 0;
  virtual void append_less_equal(AddressPtr left, AddressPtr right,
                                 TemporaryAddressPtr result) = 0;
  virtual AddressPtr append_assign(AddressPtr dest, AddressPtr src) = 0;
  virtual LabelPtr new_label(const std::string &name = "") = 0;
  virtual void append_if_true_goto(AddressPtr expression,
                                   LabelPtr target_label) = 0;
  virtual void append_if_false_goto(AddressPtr expression,
                                    LabelPtr target_label) = 0;
  virtual void append_goto(LabelPtr target_label) = 0;
  virtual void append_label(LabelPtr label) = 0;
  virtual void append_nope() = 0;
  virtual void append_function(FunctionAddressPtr left) = 0;
  virtual void append_alloca(AddressPtr variable, uint32_t bytes) = 0;
  virtual std::shared_ptr<std::vector<SiiIRCodePtr>> finish() = 0;
};

CodeBuilderPtr CreateCodeBuilder();
} // namespace front
