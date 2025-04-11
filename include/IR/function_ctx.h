#pragma once
#include "IR/type.h"
#include "IR/value.h"

namespace SiiIR {
  struct FunctionContext;
  typedef std::shared_ptr< FunctionContext > FunctionContextPtr;
  struct FunctionContext {
    TypePtr                 function_type_;
    std::vector< ValuePtr > parameters_;

    FunctionContext( TypePtr function_type )
        : function_type_( std::move( function_type ) ) {}
  };

}  // namespace SiiIR
