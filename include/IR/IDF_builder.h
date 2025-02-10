#pragma once
#include "IR/function.h"
#include <set>

namespace SiiIR {
struct IDFBuilder {
  FunctionPtr func_;
  virtual ~IDFBuilder() = default;
  IDFBuilder(FunctionPtr func) : func_(std::move(func)) {}
  virtual std::set<const BasicGroup*> get_DF(const BasicGroup*) = 0;
  virtual std::set<const BasicGroup*> get_IDF(const std::vector<const BasicGroup*>&) = 0;
};

std::unique_ptr<IDFBuilder> CreateIDFBuilder(FunctionPtr func);

} // namespace SiiIR