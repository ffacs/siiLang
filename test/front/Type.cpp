#include "gtest/gtest.h"
#include "front/type.h"

TEST(Type, TrimBuildingTypeForInt) {
  auto int_type = Type::basic(TypeKind::INT);
  EXPECT_EQ(*Type::TrimBuildingType(int_type), 
            *Type::basic(TypeKind::INT));
}

TEST(Type, TrimBuildingTypeForPointer) {
  auto int_type = Type::basic(TypeKind::INT);
  auto builder = CreateTypeBuilder(int_type);
  auto building_int = Type::building(builder);
  EXPECT_EQ(*Type::TrimBuildingType(Type::pointer(building_int)), 
            *Type::pointer(Type::basic(TypeKind::INT)));

  EXPECT_EQ(*Type::TrimBuildingType(Type::pointer(building_int, 1)), 
            *Type::pointer(Type::basic(TypeKind::INT), 1));
}

TEST(Type, TrimBuildingTypeForArray) {
  auto int_type = Type::basic(TypeKind::INT);
  auto builder = CreateTypeBuilder(int_type);
  auto building_int = Type::building(builder);
  EXPECT_EQ(*Type::TrimBuildingType(Type::array(building_int, 1)), 
            *Type::array(Type::basic(TypeKind::INT), 1));
  EXPECT_EQ(*Type::TrimBuildingType(Type::array(building_int, ArrayType::ELEMENT_COUNT_UNKOWN)), 
            *Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN));
}

TEST(Type, TrimBuildingTypeForFunction) {
  auto int_type = Type::basic(TypeKind::INT);
  auto builder = CreateTypeBuilder(int_type);
  auto building_int = Type::building(builder);

  EXPECT_EQ(*Type::TrimBuildingType(Type::function(building_int, {})), 
            *Type::function(Type::basic(TypeKind::INT), {}));

  EXPECT_EQ(*Type::TrimBuildingType(
              Type::function(building_int, {
                Declarator::Create(nullptr, "ident1"),
                Declarator::Create(building_int, "ident2")})), 
            *Type::function(Type::basic(TypeKind::INT), {
                Declarator::Create(nullptr, "ident1"),
                Declarator::Create(int_type, "ident2")}));
}

TEST(Type, TrimBuildingTypeForBuilding) {
  auto int_type = Type::basic(TypeKind::INT);
  auto building_int = Type::building(CreateTypeBuilder(int_type));
  auto building_building_int = Type::building(CreateTypeBuilder(building_int));

  EXPECT_EQ(*Type::TrimBuildingType(building_building_int), 
            *Type::basic(TypeKind::INT)); 
}

TEST(Type, NormalizePointerType) {
  EXPECT_EQ(
    *Type::NormalizePointer(Type::pointer(Type::basic(TypeKind::INT))),
    *Type::NormalizePointer(Type::pointer(Type::basic(TypeKind::INT))));

  EXPECT_EQ(
    *Type::NormalizePointer(Type::pointer(Type::basic(TypeKind::INT), 10)),
    *Type::NormalizePointer(Type::pointer(Type::basic(TypeKind::INT), 10)));
}

TEST(Type, NormalizeArrayType) {
  EXPECT_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::building(CreateTypeBuilder(Type::basic(TypeKind::INT))),
                      ArrayType::ELEMENT_COUNT_UNKOWN), true), 
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN), true), 
      std::invalid_argument);

  EXPECT_NO_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN), false));

  EXPECT_NO_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::basic(TypeKind::INT), 1), true));

  EXPECT_NO_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::basic(TypeKind::INT), 1), false));

  EXPECT_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN), 1), true), 
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN), 1), false), 
          std::invalid_argument);

  EXPECT_NO_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::array(Type::basic(TypeKind::INT), 1), 1), true));

  EXPECT_THROW(
      Type::NormalizeArrayType(
         Type::array(Type::function(Type::basic(TypeKind::INT), {}), 1), false), 
      std::invalid_argument);
}

TEST(Type, NormalizeFunctionType) {
  EXPECT_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::building(CreateTypeBuilder(Type::basic(TypeKind::INT))), {})),
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::function(Type::basic(TypeKind::INT), {}), {})),
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::array(Type::basic(TypeKind::INT), 1), {})),
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN), {})),
      std::invalid_argument);

  EXPECT_NO_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::pointer(Type::function(Type::basic(TypeKind::INT), {})), {})));

  EXPECT_NO_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::pointer(Type::array(Type::basic(TypeKind::INT), 1)), {})));

  EXPECT_NO_THROW(
      Type::NormalizeFunctionType(
         Type::function(Type::pointer(Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN)), {})));
  
  EXPECT_EQ(
      *Type::NormalizeFunctionType(
        Type::function(Type::basic(TypeKind::INT), {
          Declarator::Create(Type::function(Type::basic(TypeKind::INT), {}), "")})),
      *Type::function(Type::basic(TypeKind::INT), {
          Declarator::Create(Type::pointer(Type::function(Type::basic(TypeKind::INT), {})), "")}));

  EXPECT_EQ(
      *Type::NormalizeFunctionType(
        Type::function(Type::basic(TypeKind::INT), {
          Declarator::Create(Type::array(Type::basic(TypeKind::INT), 5), "")})),
      *Type::function(Type::basic(TypeKind::INT), {
          Declarator::Create(Type::pointer(Type::basic(TypeKind::INT), 5), "")}));
}

TEST(Type, NormalizeParameterDeclaration) {
  EXPECT_NO_THROW(
      Type::NormalizeParameterDeclaration(
         Type::array(Type::basic(TypeKind::INT), 10)));

  EXPECT_NO_THROW(
      Type::NormalizeParameterDeclaration(
         Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN)));

  EXPECT_THROW(
      Type::NormalizeParameterDeclaration(
         Type::array(
           Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN),
         ArrayType::ELEMENT_COUNT_UNKOWN)),
       std::invalid_argument);

  EXPECT_NO_THROW(
      Type::NormalizeParameterDeclaration(
         Type::array(
           Type::array(Type::basic(TypeKind::INT), 1),
         ArrayType::ELEMENT_COUNT_UNKOWN)));

  EXPECT_THROW(
      Type::NormalizeParameterDeclaration(Type::building(CreateTypeBuilder(Type::basic(TypeKind::INT)))),
      std::invalid_argument);
}

TEST(Type, NormalizeVariableDeclaration) {
  EXPECT_NO_THROW(
      Type::NormalizeVariableDeclaration(
         Type::array(Type::basic(TypeKind::INT), 10)));

  EXPECT_THROW(
      Type::NormalizeVariableDeclaration(
         Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN)),
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeVariableDeclaration(
         Type::array(
           Type::array(Type::basic(TypeKind::INT), ArrayType::ELEMENT_COUNT_UNKOWN),
         ArrayType::ELEMENT_COUNT_UNKOWN)),
       std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeVariableDeclaration(
         Type::array(
           Type::array(Type::basic(TypeKind::INT), 1),
         ArrayType::ELEMENT_COUNT_UNKOWN)),
      std::invalid_argument);

  EXPECT_THROW(
      Type::NormalizeVariableDeclaration(Type::building(CreateTypeBuilder(Type::basic(TypeKind::INT)))),
      std::invalid_argument);
}
