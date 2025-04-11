#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "IR/ID_allocator.h"
#include "IR/type.h"
#include "utils/list.h"

namespace SiiIR {
  struct FunctionContext;
  typedef std::shared_ptr< FunctionContext > FunctionContextPtr;
  enum class ValueKind : uint32_t {
    INSTRUCTION = 0,
    CONSTANT    = 1,
    PARAMETER   = 2,
    FUNCTION    = 3,
    LABEL       = 4,
    UNDEF       = 5
  };

  struct Use;
  struct SiiIRCode;
  struct Value;
  struct ConstantValue;
  struct FunctionValue;
  struct UndefValue;
  struct Label;
  struct LabelFuture;
  using SiiIRCodePtr = std::shared_ptr< SiiIRCode >;
  typedef std::shared_ptr< Value >         ValuePtr;
  typedef std::shared_ptr< ConstantValue > ConstantValuePtr;
  typedef std::shared_ptr< FunctionValue > FunctionValuePtr;
  typedef std::shared_ptr< UndefValue >    UndefValuePtr;
  typedef std::shared_ptr< Label >         LabelPtr;
  typedef std::shared_ptr< LabelFuture >   LabelFuturePtr;

  struct Value {
    explicit Value( ValueKind kind, TypePtr type )
        : kind_( kind )
        , type_( type ) {}
    ValueKind   kind_;
    TypePtr     type_;
    List< Use > users_;

    virtual std::string     to_string( IDAllocator& id_allocator ) const = 0;
    static ConstantValuePtr constant( const std::string& literal,
                                      TypePtr            type );
    static UndefValuePtr    undef( TypePtr type );
    static FunctionValuePtr
    Function( std::shared_ptr< std::vector< SiiIRCodePtr > > codes,
              FunctionContextPtr                             ctx,
              const std::string&                             name,
              TypePtr                                        type );
  };

  struct ParameterValue : public Value {
    explicit ParameterValue( TypePtr type )
        : Value( ValueKind::PARAMETER, std::move( type ) ) {}

    std::string to_string( IDAllocator& id_allocator ) const override;
  };

  struct ConstantValue : public Value {
    explicit ConstantValue( const std::string& literal, TypePtr type )
        : Value( ValueKind::CONSTANT, std::move( type ) )
        , literal_( literal ) {
      kind_ = ValueKind::CONSTANT;
    }

    std::string to_string( IDAllocator& id_allocator ) const override;
    std::string literal_;
  };

  struct UndefValue : public Value {
    explicit UndefValue( TypePtr type )
        : Value( ValueKind::UNDEF, std::move( type ) ) {}
    std::string to_string( IDAllocator& id_allocator ) const override;
  };

  struct FunctionValue : public Value {
    FunctionValue( std::shared_ptr< std::vector< SiiIRCodePtr > > codes,
                   FunctionContextPtr                             ctx,
                   std::string                                    name,
                   TypePtr                                        type )
        : Value( ValueKind::FUNCTION, std::move( type ) )
        , codes_( std::move( codes ) )
        , ctx_( std::move( ctx ) )
        , name_( std::move( name ) ) {}

    std::string to_string( IDAllocator& id_allocator ) const override;
    std::shared_ptr< std::vector< SiiIRCodePtr > > codes_;
    FunctionContextPtr                             ctx_;
    std::string                                    name_;
  };

  struct Label : public Value {
    SiiIRCode* dest_code_;
    Label( SiiIRCode* dest = nullptr )
        : Value( ValueKind::LABEL, nullptr )
        , dest_code_( dest ) {}

    std::string to_string( IDAllocator& id_allocator ) const override;
  };

}  // namespace SiiIR
