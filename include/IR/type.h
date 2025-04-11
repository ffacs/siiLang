#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace SiiIR {
  struct Type;
  using TypePtr = std::shared_ptr< Type >;
  struct Type {
    enum class Kind {
      INT      = 0,
      POINTER  = 1,
      ARRAY    = 2,
      FUNCTION = 3,
    };
    Kind kind_;
    Type( Kind kind )
        : kind_( kind ) {}
    bool operator!=( const Type& other ) const {
      return !( this->operator==( other ) );
    }
    virtual bool operator==( const Type& ) const = 0;

    static TypePtr Integer( size_t num_bits );
    static TypePtr Pointer( TypePtr aim_type, uint64_t offset_limit );
    static TypePtr Pointer( TypePtr aim_type );
    static TypePtr Array( TypePtr element_type, int64_t element_count );
    static TypePtr Function( TypePtr                return_type,
                             std::vector< TypePtr > parameter_types );
    static TypePtr GetAimType( TypePtr pointer_type );
  };

  struct IntegerType : public Type {
    size_t num_bits_;
    IntegerType( size_t num_bits )
        : Type( Type::Kind::INT )
        , num_bits_( num_bits ) {}
    bool operator==( const Type& ) const override;
  };

  struct PointerType : public Type {
    enum class OffsetLimitKind {
      kOffsetUnLimit = 0,
      kOffsetLimit   = 1,
    };
    TypePtr         aim_type_;
    OffsetLimitKind offset_limit_kind_;
    uint64_t        offset_limit_;

    PointerType( TypePtr aim_type, uint64_t offset_limit )
        : Type( Type::Kind::POINTER )
        , aim_type_( std::move( aim_type ) )
        , offset_limit_kind_( OffsetLimitKind::kOffsetLimit )
        , offset_limit_( offset_limit ) {}

    PointerType( TypePtr aim_type )
        : Type( Type::Kind::POINTER )
        , aim_type_( std::move( aim_type ) )
        , offset_limit_kind_( OffsetLimitKind::kOffsetUnLimit )
        , offset_limit_( 0 ) {}

    bool operator==( const Type& ) const override;
  };

  struct ArrayType : public Type {
    TypePtr element_type_;
    int64_t element_count_;
    ArrayType( TypePtr element_type, int64_t element_count )
        : Type( Type::Kind::ARRAY )
        , element_type_( std::move( element_type ) )
        , element_count_( element_count ) {}
    bool operator==( const Type& ) const override;
  };

  struct FunctionType : public Type {
    TypePtr                return_type_;
    std::vector< TypePtr > parameter_types_;
    FunctionType( TypePtr return_type, std::vector< TypePtr > parameter_types )
        : Type( Type::Kind::FUNCTION )
        , return_type_( std::move( return_type ) )
        , parameter_types_( std::move( parameter_types ) ) {}
    bool operator==( const Type& ) const override;
  };

}  // namespace SiiIR
