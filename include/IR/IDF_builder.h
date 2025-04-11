#pragma once
#include "IR/dominator_tree.h"
#include "IR/function.h"
#include <set>

namespace SiiIR {
struct IDFBuilder {
  FunctionPtr func_;
  virtual ~IDFBuilder() = default;
  IDFBuilder( FunctionPtr func )
      : func_( std::move( func ) ) {}
  virtual DominatorTreePtr        get_dom()                   = 0;
  virtual std::set< BasicGroup* > get_DF( const BasicGroup* ) = 0;
  virtual std::set< BasicGroup* > get_IDF( const std::vector< BasicGroup* >& )
      = 0;
};

std::unique_ptr< IDFBuilder > CreateIDFBuilder( FunctionPtr func );

}  // namespace SiiIR
