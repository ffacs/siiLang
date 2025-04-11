#include "IR/ID_allocator.h"
#include "IR/value.h"

namespace SiiIR {

std::string IDAllocator::alloc( const Value* v ) {
  if ( v->kind_ == ValueKind::CONSTANT ) {
    return static_cast< const ConstantValue* >( v )->literal_;
  } else if ( v->kind_ == ValueKind::UNDEF ) {
    return "undef";
  } else if ( v->kind_ == ValueKind::LABEL ) {
    return "Label." + std::to_string( alloc_id( v ) );
  } else {
    return "%" + std::to_string( alloc_id( v ) );
  }
}

}  // namespace SiiIR
