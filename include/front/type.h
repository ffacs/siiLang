#pragma once
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "IR/type.h"

namespace front {
enum class TypeKind : uint32_t {
  BOOL,
  INT,
  POINTER,
  FUNCTION,
  ARRAY
};

class Type;
class TypeTransformer;
class NestedTypeTransformer;
struct Declarator;
class PointerType;
class FunctionType;
class ArrayType;
typedef std::shared_ptr<Type>                  TypePtr;
typedef std::shared_ptr<TypeTransformer>       TypeTransformerPtr;
typedef std::shared_ptr<NestedTypeTransformer> NestedTypeTransformerPtr;
typedef std::shared_ptr<Declarator>            DeclaratorPtr;

struct Type {
  Type(TypeKind kind)
      : kind_(kind) {}
  TypeKind kind_;

  virtual std::string to_string(const std::string& current) const;
  virtual bool        operator==(const Type& other) const;
  bool operator!=(const Type& other) const { return !(*this == other); }
  static TypePtr DefaultType();
  static TypePtr Basic(TypeKind kind);
  static TypePtr Pointer(TypePtr aim_type, uint64_t offset_limit);
  static TypePtr Pointer(TypePtr aim_type);
  static TypePtr Function(TypePtr                    return_type,
                          std::vector<DeclaratorPtr> parameter_types);
  static TypePtr Array(TypePtr element_type, int64_t element_count);
  static TypePtr NormalizePointer(const TypePtr& type);
  static TypePtr NormalizeArrayType(const TypePtr& type, bool force_count);
  static TypePtr NormalizeFunctionType(const TypePtr& type);
  static TypePtr NormalizeParameterDeclaration(const TypePtr& type);
  static TypePtr NormalizeVariableDeclaration(const TypePtr& type);
  static size_t  SizeOf(const TypePtr& type);
  static TypePtr RemovePointer(const TypePtr& type);
  static SiiIR::TypePtr ToIRType(const TypePtr& type);
};

struct PointerType : public Type {
  TypePtr                   aim_type_;
  uint64_t                  offset_limit_;
  constexpr static uint64_t OFFSET_UNLIMIT
      = std::numeric_limits<uint64_t>::max();
  PointerType(TypePtr aim_type, uint64_t offset_limit)
      : Type(TypeKind::POINTER)
      , aim_type_(std::move(aim_type))
      , offset_limit_(offset_limit) {}

  std::string to_string(const std::string& current) const override;
  bool        operator==(const Type& other) const override;
};

struct FunctionType : public Type {
  TypePtr                    return_type_;
  std::vector<DeclaratorPtr> parameter_types_;
  FunctionType(TypePtr return_type, std::vector<DeclaratorPtr> parameter_types)
      : Type(TypeKind::FUNCTION)
      , return_type_(std::move(return_type))
      , parameter_types_(std::move(parameter_types)) {}
  std::string to_string(const std::string& current) const override;
  bool        operator==(const Type& other) const override;
};

struct ArrayType : public Type {
  TypePtr                  element_type_;
  int64_t                  element_count_;
  constexpr static int64_t ELEMENT_COUNT_UNKOWN = -1;
  ArrayType(TypePtr element_type, int64_t element_count)
      : Type(TypeKind::ARRAY)
      , element_type_(std::move(element_type))
      , element_count_(element_count) {}
  std::string to_string(const std::string& current) const override;
  bool        operator==(const Type& other) const override;
};

class NestedTypeTransformer {
public:
  virtual ~NestedTypeTransformer()     = default;
  virtual TypePtr accept(TypePtr type) = 0;
};

class PointerTypeTransformer : public NestedTypeTransformer {
public:
  TypePtr accept(TypePtr type) override { return Type::Pointer(type); }
};

class ArrayTypeTransformer : public NestedTypeTransformer {
public:
  ArrayTypeTransformer(int64_t element_count)
      : element_count_(element_count) {}
  TypePtr accept(TypePtr type) override {
    return Type::Array(type, element_count_);
  }

private:
  int64_t element_count_;
};

class FunctionTypeTransformer : public NestedTypeTransformer {
public:
  FunctionTypeTransformer(std::vector<DeclaratorPtr> parameters)
      : parameters_(std::move(parameters)) {}
  TypePtr accept(TypePtr type) override {
    return Type::Function(type, parameters_);
  }

private:
  std::vector<DeclaratorPtr> parameters_;
};

class TypeTransformer {
public:
  TypeTransformer() = default;
  void push(NestedTypeTransformerPtr builder) { builders_.push_back(builder); }
  void push(TypeTransformerPtr other) {
    builders_.insert(
        builders_.end(), other->builders_.begin(), other->builders_.end());
  }
  TypePtr accept(TypePtr type) {
    for(auto& builder: builders_) {
      type = builder->accept(type);
    }
    return type;
  }

private:
  std::vector<NestedTypeTransformerPtr> builders_;
};

struct Declarator {
  TypePtr     type_;
  std::string identifier_;
  std::string to_string() const;
  Declarator(TypePtr type, const std::string& identifier)
      : type_(std::move(type))
      , identifier_(identifier) {}
  bool operator==(const Declarator& other) const {
    if(type_ == nullptr) {
      return other.type_ == nullptr;
    }
    if(type_ != nullptr && other.type_ == nullptr) {
      return false;
    }
    return *type_ == *other.type_;
  }
  bool operator!=(const Declarator& other) const { return !(*this == other); }
  static DeclaratorPtr Create(TypePtr type, const std::string& identifier);
};

}  // namespace front
