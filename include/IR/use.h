#pragma once

#include "utils/list.h"

#pragma once

namespace SiiIR {

  struct SiiIRCode;
  struct Value;
  struct Use;
  using ValuePtr = std::shared_ptr< Value >;
  using UsePtr   = std::shared_ptr< Use >;

  struct Use : public ListNode< Use > {
    Use( SiiIRCode* user, ValuePtr value )
        : user_( user )
        , value_( std::move( value ) ) {}
    ~Use() {}
    SiiIRCode*     user_;
    const ValuePtr value_;
  };

  // Create a use only, would not add to the value's use list
  UsePtr NewUse( SiiIRCode* user, ValuePtr value );

}  // namespace SiiIR
