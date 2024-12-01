#pragma once
#include <memory>
#include <string>
#include <vector>

enum class TypeKind : uint32_t {
  BUILDING = 0,
  INT = 1,
  POINTER = 2,
  FUNCTION = 3,
  ARRAY = 4
};

class Type;
class TypeBuilder;
typedef std::shared_ptr<Type> TypePtr;
typedef std::shared_ptr<TypeBuilder> TypeBuilderPtr;

class TypeBuilder {
 public:
  virtual ~TypeBuilder() {}
  virtual void pointer_of() = 0;
  virtual void return_of(std::vector<TypePtr> parameter_types) = 0;
  virtual void array_of(size_t element_count) = 0;
  virtual TypeBuilderPtr building_of() = 0;
  virtual TypePtr get() const = 0;
};

struct Type {
  Type(TypeKind kind) : kind_(kind) {}
  TypeKind kind_;

  virtual std::string to_string(const std::string& current) const;
  virtual bool operator==(const Type& other) const;
  bool operator!=(const Type& other) const {
    return !(*this == other);
  }
  static TypePtr basic(TypeKind kind);
  static TypePtr building(TypeBuilderPtr builder);
  static TypePtr poniter(TypePtr aim_type);
  static TypePtr function(TypePtr return_type, std::vector<TypePtr> parameter_types);
  static TypePtr array(TypePtr element_type, size_t element_count);
};

struct BuildingType : public Type {
  TypeBuilderPtr builder_;
  mutable TypePtr real_type_;
  BuildingType(TypeBuilderPtr builder) 
    : Type(TypeKind::BUILDING),
      builder_(std::move(builder)) {}
  TypePtr get_real() const;

  std::string to_string(const std::string& current) const override;
  bool operator==(const Type& other) const override;
};

struct PointerType : public Type {
  TypePtr aim_type_;
  PointerType(TypePtr aim_type) 
    : Type(TypeKind::POINTER),
      aim_type_(std::move(aim_type)) {}

  std::string to_string(const std::string& current) const override;
  bool operator==(const Type& other) const override;
};

struct FunctionType : public Type { 
  TypePtr return_type_;
  std::vector<TypePtr> parameter_types_;
  FunctionType(TypePtr return_type, std::vector<TypePtr> parameter_types) 
    : Type(TypeKind::FUNCTION),
      return_type_(std::move(return_type)),
      parameter_types_(std::move(parameter_types)) {}
  std::string to_string(const std::string& current) const override;
  bool operator==(const Type& other) const override;
};

struct ArrayType : public Type {
  TypePtr element_type_;
  size_t element_count_;
  ArrayType(TypePtr element_type, size_t element_count) 
    : Type(TypeKind::ARRAY),
      element_type_(std::move(element_type)),
      element_count_(element_count) {}
  std::string to_string(const std::string& current) const override;
  bool operator==(const Type& other) const override;
};

TypeBuilderPtr CreateTypeBuilder(TypePtr base);
