#include "IR/type.h"
#include <stdexcept>

namespace SiiIR {

  TypePtr Type::Integer( size_t num_bits ) {
    return std::make_shared< IntegerType >( num_bits );
  }

  TypePtr Type::Pointer( TypePtr aim_type, uint64_t offset_limit ) {
    return std::make_shared< PointerType >( aim_type, offset_limit );
  }

  TypePtr Type::Pointer( TypePtr aim_type ) {
    return std::make_shared< PointerType >( aim_type );
  }

  TypePtr Type::Array( TypePtr element_type, int64_t element_count ) {
    return std::make_shared< ArrayType >( element_type, element_count );
  }

  TypePtr Type::Function( TypePtr                return_type,
                          std::vector< TypePtr > parameter_types ) {
    return std::make_shared< FunctionType >( return_type, parameter_types );
  }

  TypePtr Type::GetAimType( TypePtr pointer_type ) {
    if ( pointer_type->kind_ != Type::Kind::POINTER ) {
      throw std::invalid_argument( "Parameter of phi is not a address" );
    }

    return static_cast< PointerType* >( pointer_type.get() )->aim_type_;
  }

  // operator overloading
  bool IntegerType::operator==( const Type& other ) const {
    if ( other.kind_ != Type::Kind::INT ) {
      return false;
    }
    const IntegerType& other_int = static_cast< const IntegerType& >( other );
    return num_bits_ == other_int.num_bits_;
  }

  bool PointerType::operator==( const Type& other ) const {
    if ( other.kind_ != Type::Kind::POINTER ) {
      return false;
    }
    const PointerType& other_ptr = static_cast< const PointerType& >( other );
    return *aim_type_ == *other_ptr.aim_type_
           && offset_limit_ == other_ptr.offset_limit_;
  }

  bool ArrayType::operator==( const Type& other ) const {
    if ( other.kind_ != Type::Kind::ARRAY ) {
      return false;
    }
    const ArrayType& other_arr = static_cast< const ArrayType& >( other );
    return *element_type_ == *other_arr.element_type_
           && element_count_ == other_arr.element_count_;
  }

  bool FunctionType::operator==( const Type& other ) const {
    if ( other.kind_ != Type::Kind::FUNCTION ) {
      return false;
    }
    const FunctionType& other_func
        = static_cast< const FunctionType& >( other );
    if ( parameter_types_.size() != other_func.parameter_types_.size() ) {
      return false;
    }
    for ( size_t i = 0; i < parameter_types_.size(); ++i ) {
      if ( *parameter_types_[ i ] != *other_func.parameter_types_[ i ] ) {
        return false;
      }
    }
    return *return_type_ == *other_func.return_type_;
  }
}  // namespace SiiIR
