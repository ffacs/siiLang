#include "type.h"
#include <stdexcept>
#include <iostream>

TypePtr Type::basic(TypeKind kind) {
  return std::make_shared<Type>(kind);
}

TypePtr Type::building(TypeBuilderPtr builder) {
  return std::make_shared<BuildingType>(std::move(builder));
}

TypePtr Type::poniter(TypePtr aim_type) {
  return std::make_shared<PointerType>(std::move(aim_type));
}

TypePtr Type::function(TypePtr return_type, std::vector<TypePtr> parameter_types) {
  return std::make_shared<FunctionType>(std::move(return_type), std::move(parameter_types));
}

TypePtr Type::array(TypePtr element_type, size_t element_count) {
  return std::make_shared<ArrayType>(std::move(element_type), element_count);
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
  return *get_real() == other;
}

bool PointerType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = reinterpret_cast<const PointerType&>(other);
  return *aim_type_ == *typed_other.aim_type_;
}

bool FunctionType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = reinterpret_cast<const FunctionType&>(other);
  if (*return_type_ != *typed_other.return_type_) { return false; }
  if (parameter_types_.size() != typed_other.parameter_types_.size()) {
    return false;
  }
  for (size_t i = 0; i < parameter_types_.size(); i++) {
    if (*parameter_types_[i] == *typed_other.parameter_types_[i]) {
      return false;
    }
  }
  return true;
}

bool ArrayType::operator==(const Type &other) const {
  if (!Type::operator==(other)) { return false; }
  auto typed_other = reinterpret_cast<const ArrayType&>(other);
  if (element_count_ != typed_other.element_count_) { return false; }
  return *element_type_ == *typed_other.element_type_;
}

std::string PointerType::to_string(const std::string& current) const {
  return aim_type_->to_string("(*" + current + ")");
}

std::string FunctionType::to_string(const std::string& current) const {
  auto result = "(" + return_type_->to_string("") + "(" + current + ")" + "(";
  for (size_t i = 0; i < parameter_types_.size(); i++) {
    result += parameter_types_[i]->to_string(std::string("arg") + std::to_string(i));
    result += (i + 1 == parameter_types_.size()) ? ")" : ",";
  }
  if (parameter_types_.empty()) {
    result += ")";
  }
  result += ")";
  return result;
}

std::string ArrayType::to_string(const std::string &current) const {
  return element_type_->to_string(current + "[" + std::to_string(element_count_) + "]");
}

class TypeBuilderImpl : public TypeBuilder {
 public:
  TypeBuilderImpl(TypePtr base) : current_type_(std::move(base)) {}
  void pointer_of() override;
  void return_of(std::vector<TypePtr> parameter_types) override;
  void array_of(size_t element_count) override;
  TypeBuilderPtr building_of() override;
  TypePtr get() const override;
 protected:
  TypePtr current_type_;
};

void TypeBuilderImpl::pointer_of() {
  auto new_pointer = Type::poniter(std::move(current_type_));
  current_type_ = std::move(new_pointer);
}

void TypeBuilderImpl::return_of(std::vector<TypePtr> parameter_types) {
  auto new_function = Type::function(std::move(current_type_), std::move(parameter_types));
  current_type_ = std::move(new_function);
}

void TypeBuilderImpl::array_of(size_t element_count) {
  auto new_array = Type::array(std::move(current_type_), element_count);
  current_type_ = std::move(new_array);
}

TypeBuilderPtr TypeBuilderImpl::building_of() {
  auto new_builder = CreateTypeBuilder(std::move(current_type_));
  current_type_ = Type::building(new_builder);
  return new_builder;
}

TypePtr TypeBuilderImpl::get() const {
  return current_type_;
}

TypeBuilderPtr CreateTypeBuilder(TypePtr base) {
  return std::make_shared<TypeBuilderImpl>(std::move(base));
}
