#include "IR/value.h"
#include "IR/IR.h"
#include "IR/function_ctx.h"

#include <sstream>

namespace SiiIR {
ConstantValuePtr Value::constant( const std::string& literal, TypePtr type ) {
  return std::make_shared< ConstantValue >( literal, std::move( type ) );
}

UndefValuePtr Value::undef( TypePtr type ) {
  return std::make_shared< UndefValue >( std::move( type ) );
}

std::string ParameterValue::to_string( IDAllocator& id_allocator ) const {
  return id_allocator.alloc( this );
}

std::string ConstantValue::to_string( IDAllocator& id_allocator ) const {
  return literal_;
}

std::string UndefValue::to_string( IDAllocator& id_allocator ) const {
  return "undef";
}

std::string Label::to_string( IDAllocator& id_allocator ) const {
  return id_allocator.alloc( this );
}

std::string FunctionValue::to_string( IDAllocator& id_allocator ) const {
  std::stringstream result;
  result << "@" + name_ + "(";
  for ( size_t i = 0; i < ctx_->parameters_.size(); i++ ) {
    result << id_allocator.alloc( ctx_->parameters_[ i ].get() );
    if ( i + 1 != ctx_->parameters_.size() ) {
      result << ", ";
    }
  }
  result << "):\n";
  for ( size_t i = 0; i < codes_->size(); i++ ) {
    result << ( *codes_ )[ i ]->to_string( id_allocator );
    if ( i + 1 != codes_->size() ) {
      result << "\n";
    }
  }
  return result.str();
}

FunctionValuePtr
Value::Function( std::shared_ptr< std::vector< SiiIRCodePtr > > codes,
                 FunctionContextPtr                             ctx,
                 const std::string&                             name,
                 TypePtr                                        type ) {
  return std::make_shared< FunctionValue >(
      std::move( codes ), std::move( ctx ), name, std::move( type ) );
}

}  // namespace SiiIR
