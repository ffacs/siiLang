#include "front/type.h"
#include <stdexcept>

namespace front {
TypePtr Type::DefaultType() { return Type::Basic(TypeKind::INT); }

TypePtr Type::Basic(TypeKind kind) { return std::make_shared<Type>(kind); }

TypePtr Type::Pointer(TypePtr aim_type, uint64_t offset_limit) {
  return std::make_shared<PointerType>(std::move(aim_type), offset_limit);
}
TypePtr Type::Pointer(TypePtr aim_type) {
  return std::make_shared<PointerType>(std::move(aim_type),
                                       PointerType::OFFSET_UNLIMIT);
}

TypePtr Type::Function(TypePtr                    return_type,
                       std::vector<DeclaratorPtr> parameter_types) {
  return std::make_shared<FunctionType>(std::move(return_type),
                                        std::move(parameter_types));
}

TypePtr Type::Array(TypePtr element_type, int64_t element_count) {
  return std::make_shared<ArrayType>(std::move(element_type), element_count);
}

TypePtr Type::NormalizePointer(const TypePtr& type) {
  const PointerType& pointer = static_cast<const PointerType&>(*type);
  return Type::Pointer(NormalizeParameterDeclaration(pointer.aim_type_),
                       pointer.offset_limit_);
}

TypePtr Type::NormalizeArrayType(const TypePtr& type, bool force_count) {
  const ArrayType& array = static_cast<const ArrayType&>(*type);
  if(array.element_count_ == ArrayType::ELEMENT_COUNT_UNKOWN && force_count) {
    throw std::invalid_argument("Size of array not specified");
  }
  switch(array.element_type_->kind_) {
  case TypeKind::ARRAY:
    return Type::Array(NormalizeArrayType(array.element_type_, true),
                       array.element_count_);
  case TypeKind::FUNCTION:
    throw std::invalid_argument("Element of array cannot be function");
  default:
    return Type::Array(NormalizeParameterDeclaration(array.element_type_),
                       array.element_count_);
  }
}

TypePtr Type::NormalizeFunctionType(const TypePtr& type) {
  const FunctionType& function    = static_cast<const FunctionType&>(*type);
  auto&               return_type = function.return_type_;
  TypePtr             new_return_type;
  switch(return_type->kind_) {
  case TypeKind::ARRAY:
    throw std::invalid_argument("Function cannot return a array");
  case TypeKind::FUNCTION:
    throw std::invalid_argument("Function cannot return a function");
  default: new_return_type = NormalizeParameterDeclaration(return_type);
  }
  std::vector<DeclaratorPtr> new_parameters;
  for(const auto& parameter: function.parameter_types_) {
    auto new_parameter_type = NormalizeParameterDeclaration(parameter->type_);
    if(parameter->type_->kind_ == TypeKind::FUNCTION) {
      new_parameter_type = Type::Pointer(std::move(new_parameter_type),
                                         PointerType::OFFSET_UNLIMIT);
    } else if(parameter->type_->kind_ == TypeKind::ARRAY) {
      auto&    array_type    = static_cast<const ArrayType&>(*parameter->type_);
      auto     element_type  = array_type.element_type_;
      auto     element_count = array_type.element_count_;
      uint64_t offset_limit  = PointerType::OFFSET_UNLIMIT;
      if(element_count != ArrayType::ELEMENT_COUNT_UNKOWN) {
        offset_limit = element_count;
      }
      new_parameter_type = Type::Pointer(element_type, offset_limit);
    }

    new_parameters.emplace_back(Declarator::Create(
        std::move(new_parameter_type), parameter->identifier_));
  }
  return Type::Function(std::move(new_return_type), std::move(new_parameters));
}

TypePtr Type::NormalizeParameterDeclaration(const TypePtr& type) {
  switch(type->kind_) {
  case TypeKind::POINTER : return NormalizePointer(type);
  case TypeKind::ARRAY   : return NormalizeArrayType(type, false);
  case TypeKind::FUNCTION: return NormalizeFunctionType(type);
  case TypeKind::INT     : return type;
  }
}

TypePtr Type::NormalizeVariableDeclaration(const TypePtr& type) {
  if(type->kind_ == TypeKind::ARRAY) {
    auto array_type = static_cast<const ArrayType&>(*type);
    if(array_type.element_count_ == ArrayType::ELEMENT_COUNT_UNKOWN) {
      throw std::invalid_argument(
          "Definition of variable with array type needs an explicit size");
    }
  }
  return NormalizeParameterDeclaration(type);
}

size_t Type::SizeOf(const TypePtr& type) {
  switch(type->kind_) {
  case TypeKind::INT    : return 4;
  case TypeKind::POINTER: return 8;
  case TypeKind::ARRAY:
  case TypeKind::FUNCTION:
    throw std::invalid_argument("Unsupport type for SizeOf");
  }
}

std::string Type::to_string(const std::string& current) const {
  switch(kind_) {
  case TypeKind::INT: return std::string("int ") + current;
  default           : throw std::invalid_argument("Unkown type in Type::to_string");
  }
}

bool Type::operator==(const Type& other) const { return kind_ == other.kind_; }

std::string PointerType::to_string(const std::string& current) const {
  return aim_type_->to_string("(*" + current + ")");
}

bool PointerType::operator==(const Type& other) const {
  if(!Type::operator==(other)) {
    return false;
  }
  auto typed_other = static_cast<const PointerType&>(other);
  return *aim_type_ == *typed_other.aim_type_
         && offset_limit_ == typed_other.offset_limit_;
}

bool FunctionType::operator==(const Type& other) const {
  if(!Type::operator==(other)) {
    return false;
  }
  auto typed_other = static_cast<const FunctionType&>(other);
  if(*return_type_ != *typed_other.return_type_) {
    return false;
  }
  if(parameter_types_.size() != typed_other.parameter_types_.size()) {
    return false;
  }
  for(size_t i = 0; i < parameter_types_.size(); i++) {
    if(*parameter_types_[i] != *typed_other.parameter_types_[i]) {
      return false;
    }
  }
  return true;
}

std::string FunctionType::to_string(const std::string& current) const {
  auto result = "(" + return_type_->to_string("") + "(" + current + ")" + "(";
  for(size_t i = 0; i < parameter_types_.size(); i++) {
    result += parameter_types_[i]->to_string();
    result += (i + 1 == parameter_types_.size()) ? ")" : ",";
  }
  if(parameter_types_.empty()) {
    result += ")";
  }
  result += ")";
  return result;
}

bool ArrayType::operator==(const Type& other) const {
  if(!Type::operator==(other)) {
    return false;
  }
  auto typed_other = static_cast<const ArrayType&>(other);
  if(element_count_ != typed_other.element_count_) {
    return false;
  }
  return *element_type_ == *typed_other.element_type_;
}

std::string ArrayType::to_string(const std::string& current) const {
  return element_type_->to_string(current + "[" + std::to_string(element_count_)
                                  + "]");
}

std::string Declarator::to_string() const {
  return type_->to_string(identifier_);
}

DeclaratorPtr Declarator::Create(TypePtr type, const std::string& identifier) {
  return std::make_shared<Declarator>(std::move(type), identifier);
}

SiiIR::TypePtr Type::ToIRType(const TypePtr& type) {
  switch(type->kind_) {
  case TypeKind::INT    : return SiiIR::Type::Integer(32);
  case TypeKind::POINTER: {
    auto& pointer_type = static_cast<const PointerType&>(*type);
    if(pointer_type.offset_limit_ == PointerType::OFFSET_UNLIMIT) {
      return SiiIR::Type::Pointer(ToIRType(pointer_type.aim_type_));
    }
    return SiiIR::Type::Pointer(ToIRType(pointer_type.aim_type_),
                                pointer_type.offset_limit_);
  }
  case TypeKind::ARRAY: {
    auto& array_type = static_cast<const ArrayType&>(*type);
    return std::make_shared<SiiIR::ArrayType>(
        ToIRType(array_type.element_type_), array_type.element_count_);
  }
  case TypeKind::FUNCTION: {
    auto& function_type = static_cast<const FunctionType&>(*type);
    std::vector<SiiIR::TypePtr> parameter_types;
    for(auto& parameter: function_type.parameter_types_) {
      parameter_types.emplace_back(ToIRType(parameter->type_));
    }
    return std::make_shared<SiiIR::FunctionType>(
        ToIRType(function_type.return_type_), parameter_types);
  }
  }
}

}  // namespace front
