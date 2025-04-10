#include "front/type.h"
#include "gtest/gtest.h"

namespace front {

TEST(Type, NormalizePointerType) {
  EXPECT_EQ(*Type::NormalizePointer(Type::Pointer(Type::Basic(TypeKind::INT))),
            *Type::NormalizePointer(Type::Pointer(Type::Basic(TypeKind::INT))));

  EXPECT_EQ(
      *Type::NormalizePointer(Type::Pointer(Type::Basic(TypeKind::INT), 10)),
      *Type::NormalizePointer(Type::Pointer(Type::Basic(TypeKind::INT), 10)));
}

TEST(Type, NormalizeArrayType) {
  EXPECT_THROW(
      Type::NormalizeArrayType(Type::Array(Type::Basic(TypeKind::INT),
                                           ArrayType::ELEMENT_COUNT_UNKOWN),
                               true),
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeArrayType(Type::Array(Type::Basic(TypeKind::INT),
                                           ArrayType::ELEMENT_COUNT_UNKOWN),
                               true),
      std::invalid_argument);

  EXPECT_NO_THROW(Type::NormalizeArrayType(
      Type::Array(Type::Basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN),
      false));

  EXPECT_NO_THROW(Type::NormalizeArrayType(
      Type::Array(Type::Basic(TypeKind::INT), 1), true));

  EXPECT_NO_THROW(Type::NormalizeArrayType(
      Type::Array(Type::Basic(TypeKind::INT), 1), false));

  EXPECT_THROW(Type::NormalizeArrayType(
                   Type::Array(Type::Array(Type::Basic(TypeKind::INT),
                                           ArrayType::ELEMENT_COUNT_UNKOWN),
                               1),
                   true),
               std::invalid_argument);

  EXPECT_THROW(Type::NormalizeArrayType(
                   Type::Array(Type::Array(Type::Basic(TypeKind::INT),
                                           ArrayType::ELEMENT_COUNT_UNKOWN),
                               1),
                   false),
               std::invalid_argument);

  EXPECT_NO_THROW(Type::NormalizeArrayType(
      Type::Array(Type::Array(Type::Basic(TypeKind::INT), 1), 1), true));

  EXPECT_THROW(
      Type::NormalizeArrayType(
          Type::Array(Type::Function(Type::Basic(TypeKind::INT), {}), 1),
          false),
      std::invalid_argument);
}

TEST(Type, NormalizeFunctionType) {
  EXPECT_THROW(Type::NormalizeFunctionType(Type::Function(
                   Type::Function(Type::Basic(TypeKind::INT), {}), {})),
               std::invalid_argument);

  EXPECT_THROW(Type::NormalizeFunctionType(Type::Function(
                   Type::Array(Type::Basic(TypeKind::INT), 1), {})),
               std::invalid_argument);

  EXPECT_THROW(Type::NormalizeFunctionType(
                   Type::Function(Type::Array(Type::Basic(TypeKind::INT),
                                              ArrayType::ELEMENT_COUNT_UNKOWN),
                                  {})),
               std::invalid_argument);

  EXPECT_NO_THROW(Type::NormalizeFunctionType(Type::Function(
      Type::Pointer(Type::Function(Type::Basic(TypeKind::INT), {})), {})));

  EXPECT_NO_THROW(Type::NormalizeFunctionType(Type::Function(
      Type::Pointer(Type::Array(Type::Basic(TypeKind::INT), 1)), {})));

  EXPECT_NO_THROW(Type::NormalizeFunctionType(Type::Function(
      Type::Pointer(Type::Array(Type::Basic(TypeKind::INT),
                                ArrayType::ELEMENT_COUNT_UNKOWN)),
      {})));

  EXPECT_EQ(
      *Type::NormalizeFunctionType(Type::Function(
          Type::Basic(TypeKind::INT),
          {Declarator::Create(Type::Function(Type::Basic(TypeKind::INT), {}),
                              "")})),
      *Type::Function(Type::Basic(TypeKind::INT),
                      {Declarator::Create(Type::Pointer(Type::Function(
                                              Type::Basic(TypeKind::INT), {})),
                                          "")}));

  EXPECT_EQ(
      *Type::NormalizeFunctionType(
          Type::Function(Type::Basic(TypeKind::INT),
                         {Declarator::Create(
                             Type::Array(Type::Basic(TypeKind::INT), 5), "")})),
      *Type::Function(Type::Basic(TypeKind::INT),
                      {Declarator::Create(
                          Type::Pointer(Type::Basic(TypeKind::INT), 5), "")}));
}

TEST(Type, NormalizeParameterDeclaration) {
  EXPECT_NO_THROW(Type::NormalizeParameterDeclaration(
      Type::Array(Type::Basic(TypeKind::INT), 10)));

  EXPECT_NO_THROW(Type::NormalizeParameterDeclaration(Type::Array(
      Type::Basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN)));

  EXPECT_THROW(Type::NormalizeParameterDeclaration(
                   Type::Array(Type::Array(Type::Basic(TypeKind::INT),
                                           ArrayType::ELEMENT_COUNT_UNKOWN),
                               ArrayType::ELEMENT_COUNT_UNKOWN)),
               std::invalid_argument);

  EXPECT_NO_THROW(Type::NormalizeParameterDeclaration(
      Type::Array(Type::Array(Type::Basic(TypeKind::INT), 1),
                  ArrayType::ELEMENT_COUNT_UNKOWN)));
}

TEST(Type, NormalizeVariableDeclaration) {
  EXPECT_THROW(
      Type::NormalizeVariableDeclaration(Type::Array(
          Type::Basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN)),
      std::invalid_argument);

  EXPECT_THROW(Type::NormalizeVariableDeclaration(
                   Type::Array(Type::Array(Type::Basic(TypeKind::INT),
                                           ArrayType::ELEMENT_COUNT_UNKOWN),
                               ArrayType::ELEMENT_COUNT_UNKOWN)),
               std::invalid_argument);

  EXPECT_THROW(Type::NormalizeVariableDeclaration(
                   Type::Array(Type::Array(Type::Basic(TypeKind::INT), 1),
                               ArrayType::ELEMENT_COUNT_UNKOWN)),
               std::invalid_argument);
}

} // namespace front
