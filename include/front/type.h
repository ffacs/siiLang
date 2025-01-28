#pragma once
#include <limits>
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
struct Declarator;
class BuildingType;
class PointerType;
class FunctionType;
class ArrayType;
typedef std::shared_ptr<Type> TypePtr;
typedef std::shared_ptr<TypeBuilder> TypeBuilderPtr;
typedef std::shared_ptr<Declarator> DeclaratorPtr;

class TypeBuilder {
public:
  virtual ~TypeBuilder() {}
  virtual void
  pointer_of(uint64_t offset_limit = std::numeric_limits<uint64_t>::max()) = 0;
  virtual void return_of(std::vector<DeclaratorPtr> parameter_types) = 0;
  virtual void array_of(int64_t element_count) = 0;
  virtual TypeBuilderPtr building_of() = 0;
  virtual TypePtr get() const = 0;
};

TypeBuilderPtr CreateTypeBuilder(TypePtr base);

struct Type {
  Type(TypeKind kind) : kind_(kind) {}
  TypeKind kind_;

  virtual std::string to_string(const std::string &current) const;
  virtual bool operator==(const Type &other) const;
  bool operator!=(const Type &other) const { return !(*this == other); }
  static TypePtr default_type();
  static TypePtr basic(TypeKind kind);
  static TypePtr building(TypeBuilderPtr builder);
  static TypePtr pointer(TypePtr aim_type, uint64_t offset_limit);
  static TypePtr pointer(TypePtr aim_type);
  static TypePtr function(TypePtr return_type,
                          std::vector<DeclaratorPtr> parameter_types);
  static TypePtr array(TypePtr element_type, int64_t element_count);
  static TypePtr TrimBuildingType(const TypePtr &type);
  static TypePtr NormalizePointer(const TypePtr &type);
  static TypePtr NormalizeArrayType(const TypePtr &type, bool force_count);
  static TypePtr NormalizeFunctionType(const TypePtr &type);
  static TypePtr NormalizeParameterDeclaration(const TypePtr &type);
  static TypePtr NormalizeVariableDeclaration(const TypePtr &type);
  static size_t SizeOf(const TypePtr &type);
};

struct BuildingType : public Type {
  TypeBuilderPtr builder_;
  mutable TypePtr real_type_;
  BuildingType(TypeBuilderPtr builder)
      : Type(TypeKind::BUILDING), builder_(std::move(builder)) {}
  TypePtr get_real() const;

  std::string to_string(const std::string &current) const override;
  bool operator==(const Type &other) const override;
};

struct PointerType : public Type {
  TypePtr aim_type_;
  uint64_t offset_limit_;
  constexpr static uint64_t OFFSET_UNLIMIT =
      std::numeric_limits<uint64_t>::max();
  PointerType(TypePtr aim_type, uint64_t offset_limit)
      : Type(TypeKind::POINTER), aim_type_(std::move(aim_type)),
        offset_limit_(offset_limit) {}

  std::string to_string(const std::string &current) const override;
  bool operator==(const Type &other) const override;
};

struct FunctionType : public Type {
  TypePtr return_type_;
  std::vector<DeclaratorPtr> parameter_types_;
  FunctionType(TypePtr return_type, std::vector<DeclaratorPtr> parameter_types)
      : Type(TypeKind::FUNCTION), return_type_(std::move(return_type)),
        parameter_types_(std::move(parameter_types)) {}
  std::string to_string(const std::string &current) const override;
  bool operator==(const Type &other) const override;
};

struct ArrayType : public Type {
  TypePtr element_type_;
  int64_t element_count_;
  constexpr static int64_t ELEMENT_COUNT_UNKOWN = -1;
  ArrayType(TypePtr element_type, int64_t element_count)
      : Type(TypeKind::ARRAY), element_type_(std::move(element_type)),
        element_count_(element_count) {}
  std::string to_string(const std::string &current) const override;
  bool operator==(const Type &other) const override;
};

struct Declarator {
  TypePtr type_;
  std::string identifier_;
  std::string to_string() const;
  Declarator(TypePtr type, const std::string &identifier)
      : type_(std::move(type)), identifier_(identifier) {}
  bool operator==(const Declarator &other) const {
    if (type_ == nullptr) {
      return other.type_ == nullptr;
    }
    if (type_ != nullptr && other.type_ == nullptr) {
      return false;
    }
    return *type_ == *other.type_;
  }
  bool operator!=(const Declarator &other) const { return !(*this == other); }
  static DeclaratorPtr Create(TypePtr type, const std::string &identifier);
};
