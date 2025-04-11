#pragma once
#include "IR/IR.h"
#include "IR/function_ctx.h"

namespace SiiIR {

  struct BasicGroup {
    List< SiiIRCode > codes_;

    std::vector< BasicGroup* > precedes_;
    std::vector< BasicGroup* > follows_;
    LabelPtr                   label_;
    std::string                to_string( IDAllocator& ) const;
  };

  using BasicGroupPtr = std::shared_ptr< BasicGroup >;

  struct Function {
    FunctionContextPtr           ctx_;
    std::vector< BasicGroupPtr > basic_groups_;
    BasicGroup*                  entry_;
    std::string                  name_;

    std::string to_string() const;
  };

  using FunctionPtr = std::shared_ptr< Function >;

  FunctionPtr BuildFunction( std::vector< SiiIRCodePtr > codes,
                             FunctionContextPtr          ctx,
                             std::string                 name );
  // TODO merge BasicGroup with its following BasicGroup when there is only one

}  // namespace SiiIR
