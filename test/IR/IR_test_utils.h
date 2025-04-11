#pragma once
#include "IR/function.h"
#include <map>
#include <set>

namespace SiiIR {

FunctionPtr
BuildFunction(size_t node_count, size_t extra_edge_count, bool random = true);
std::map<const BasicGroup*, std::set<const BasicGroup*>>
GetDominators(FunctionPtr func);

}  // namespace SiiIR
