#include "type.h"
#include <stdexcept>
#include <iostream>

class TypeBuilderImpl : public TypeBuilder {
 public:
  TypeBuilderImpl(TypePtr base) : current_type_(std::move(base)) {}
  void pointer_of(uint64_t offset_limit) override;
  void return_of(std::vector<DeclaratorPtr> parameter_types) override;
  void array_of(int64_t element_count) override;
  TypeBuilderPtr building_of() override;
  TypePtr get() const override;
 protected:
  TypePtr current_type_;
};

void TypeBuilderImpl::pointer_of(uint64_t offset_limit) {
  auto new_pointer = Type::pointer(std::move(current_type_), offset_limit);
  current_type_ = std::move(new_pointer);
}

void TypeBuilderImpl::return_of(std::vector<DeclaratorPtr> parameter_types) {
  auto new_function = Type::function(std::move(current_type_), std::move(parameter_types));
  current_type_ = std::move(new_function);
}

void TypeBuilderImpl::array_of(int64_t element_count) {
  auto new_array = Type::array(std::move(current_type_), element_count);
  current_type_ = std::move(new_array);
}

TypeBuilderPtr TypeBuilderImpl::building_of() {
  auto new_builder = CreateTypeBuilder(std::move(current_type_));
  current_type_ = Type::building(new_builder);
  return new_builder;
}

TypePtr TypeBuilderImpl::get() const {
  return Type::TrimBuildingType(current_type_);
}

TypeBuilderPtr CreateTypeBuilder(TypePtr base) {
  return std::make_shared<TypeBuilderImpl>(std::move(base));
}

TypePtr Type::default_type() {
  return Type::basic(TypeKind::INT);
}

TypePtr Type::basic(TypeKind kind) {
  return std::make_shared<Type>(kind);
}

TypePtr Type::building(TypeBuilderPtr builder) {
  return std::make_shared<BuildingType>(std::move(builder));
}

TypePtr Type::pointer(TypePtr aim_type, uint64_t offset_limit) {
  return std::make_shared<PointerType>(std::move(aim_type), offset_limit);
}
TypePtr Type::pointer(TypePtr aim_type) {
  return std::make_shared<PointerType>(std::move(aim_type), PointerType::OFFSET_UNLIMIT);
}

TypePtr Type::function(TypePtr return_type, std::vector<DeclaratorPtr> parameter_types) {
  return std::make_shared<FunctionType>(std::move(return_type), std::move(parameter_types));
}

TypePtr Type::array(TypePtr element_type, int64_t element_count) {
  return std::make_shared<ArrayType>(std::move(element_type), element_count);
}

TypePtr Type::TrimBuildingType(const TypePtr &type) {
  switch (type->kind_) {
    case TypeKind::POINTER: {
      const PointerType& pointer = static_cast<const PointerType&>(*type);
      return Type::pointer(TrimBuildingType(pointer.aim_type_), pointer.offset_limit_);
    }
    case TypeKind::ARRAY: {
      const ArrayType& array = static_cast<const ArrayType&>(*type);
      return Type::array(TrimBuildingType(array.element_type_), array.element_count_);
    }
    case TypeKind::FUNCTION: {
      const FunctionType& function = static_cast<const FunctionType&>(*type);
      std::vector<DeclaratorPtr> trimmed_parameters;
      trimmed_parameters.reserve(function.parameter_types_.size());
      for (const auto& parameter : function.parameter_types_) {
        if (parameter->type_ == nullptr) {
          trimmed_parameters.emplace_back(Declarator::Create(nullptr, parameter->identifier_));
        } else {
          trimmed_parameters.emplace_back(Declarator::Create(TrimBuildingType(parameter->type_), parameter->identifier_));
        }
      }
      return Type::function(TrimBuildingType(function.return_type_), std::move(trimmed_parameters));
    }
    case TypeKind::INT:
      return type;
    case TypeKind::BUILDING:
      const BuildingType& building = static_cast<const BuildingType&>(*type);
      return building.get_real();
  }
}

TypePtr Type::NormalizePointer(const TypePtr& type) {
  const PointerType& pointer = static_cast<const PointerType&>(*type);
  return Type::pointer(NormalizeParameterDeclaration(pointer.aim_type_), pointer.offset_limit_);
}

TypePtr Type::NormalizeArrayType(const TypePtr& type, bool force_count) {
  const ArrayType& array = static_cast<const ArrayType&>(*type);
  if (array.element_count_ == ArrayType::ELEMENT_COUNT_UNKOWN && force_count) {
    throw std::invalid_argument("Size of array not specified");
  }
  switch (array.element_type_->kind_) {
    case TypeKind::ARRAY:
      return Type::array(NormalizeArrayType(array.element_type_, true), array.element_count_);
    case TypeKind::FUNCTION:
      throw std::invalid_argument("Element of array cannot be function");
    default:
      return Type::array(NormalizeParameterDeclaration(array.element_type_), array.element_count_);
  }
}

TypePtr Type::NormalizeFunctionType(const TypePtr& type) {
  const FunctionType& function = static_cast<const FunctionType&>(*type);
  auto& return_type = function.return_type_;
  TypePtr new_return_type;
  switch (return_type->kind_) {
    case TypeKind::ARRAY:
      throw std::invalid_argument("Function cannot return a array");
    case TypeKind::FUNCTION:
      throw std::invalid_argument("Function cannot return a function");
    default:
      new_return_type = NormalizeParameterDeclaration(return_type);
  }
  std::vector<DeclaratorPtr> new_parameters;
  for (const auto& parameter : function.parameter_types_) {
    auto new_parameter_type = NormalizeParameterDeclaration(parameter->type_);
    if (parameter->type_->kind_ == TypeKind::FUNCTION) {
      new_parameter_type = Type::pointer(std::move(new_parameter_type), PointerType::OFFSET_UNLIMIT);
    } else if (parameter->type_->kind_ == TypeKind::ARRAY) {
      auto& array_type = static_cast<const ArrayType&>(*parameter->type_);
      auto element_type = array_type.element_type_;
      auto element_count = array_type.element_count_;
      uint64_t offset_limit = PointerType::OFFSET_UNLIMIT;
      if (element_count != ArrayType::ELEMENT_COUNT_UNKOWN) {
        offset_limit = element_count;
      }
      new_parameter_type = Type::pointer(element_type, offset_limit);
    }

    new_parameters.emplace_back(Declarator::Create(std::move(new_parameter_type), parameter->identifier_));
  }
  return Type::function(std::move(new_return_type), std::move(new_parameters));
}

TypePtr Type::NormalizeParameterDeclaration(const TypePtr& type) {
  switch (type->kind_) {
    case TypeKind::POINTER:
      return NormalizePointer(type);
    case TypeKind::ARRAY:
      return NormalizeArrayType(type, false);
    case TypeKind::FUNCTION:
      return NormalizeFunctionType(type);
    case TypeKind::INT:
      return type;
    case TypeKind::BUILDING:
      throw std::invalid_argument("Invalid Building type");
  }
}

TypePtr Type::NormalizeVariableDeclaration(const TypePtr& type) {
  if (type->kind_ == TypeKind::ARRAY) {
    auto array_type = static_cast<const ArrayType&>(*type);
    if (array_type.element_count_ == ArrayType::ELEMENT_COUNT_UNKOWN) {
      throw std::invalid_argument("Definition of variable with array type needs an explicit size");
    }
  }
  return NormalizeParameterDeclaration(type);
}

size_t Type::SizeOf(const TypePtr& type) {
  switch (type->kind_) {
    case TypeKind::INT:
      return 4;
    case TypeKind::POINTER:
      return 8;
    case TypeKind::ARRAY:
    case TypeKind::FUNCTION:
    case TypeKind::BUILDING:
      throw std::invalid_argument("Unsupport type for SizeOf");
  }
}

std::string Type::to_string(const std::string& current) const {
  switch (kind_) {
    case TypeKind::INT:
      return std::string("int ") + current;
    default:
      throw std::invalid_argument("Unkown type in Type::to_string");
  }
}

bool Type::operator==(const Type &other) const {
  return kind_ == other.kind_;
}

TypePtr BuildingType::get_real() const {
  if (real_type_ == nullptr) {
    real_type_ = builder_->get();
  } 
  return real_type_;
}

std::string BuildingType::to_string(const std::string &current) const {
  return get_real()->to_string(current);
}

bool BuildingType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = static_cast<const BuildingType&>(other);
  return *get_real() == *typed_other.get_real();
}

std::string PointerType::to_string(const std::string& current) const {
  return aim_type_->to_string("(*" + current + ")");
}

bool PointerType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = static_cast<const PointerType&>(other);
  return *aim_type_ == *typed_other.aim_type_ && offset_limit_ == typed_other.offset_limit_;
}

bool FunctionType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = static_cast<const FunctionType&>(other);
  if (*return_type_ != *typed_other.return_type_) { return false; }
  if (parameter_types_.size() != typed_other.parameter_types_.size()) {
    return false;
  }
  for (size_t i = 0; i < parameter_types_.size(); i++) {
    if (*parameter_types_[i] != *typed_other.parameter_types_[i]) {
      return false;
    }
  }
  return true;
}

std::string FunctionType::to_string(const std::string& current) const {
  auto result = "(" + return_type_->to_string("") + "(" + current + ")" + "(";
  for (size_t i = 0; i < parameter_types_.size(); i++) {
    result += parameter_types_[i]->to_string();
    result += (i + 1 == parameter_types_.size()) ? ")" : ",";
  }
  if (parameter_types_.empty()) {
    result += ")";
  }
  result += ")";
  return result;
}

bool ArrayType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = static_cast<const ArrayType&>(other);
  if (element_count_ != typed_other.element_count_) { return false; }
  return *element_type_ == *typed_other.element_type_;
}

std::string ArrayType::to_string(const std::string &current) const {
  return element_type_->to_string(current + "[" + std::to_string(element_count_) + "]");
}

std::string Declarator::to_string() const {
  return type_->to_string(identifier_);
}

DeclaratorPtr Declarator::Create(TypePtr type, const std::string& identifier) {
  return std::make_shared<Declarator>(std::move(type), identifier);
}
