#pragma once
#include <string>
#include <memory>
#include <vector>
#include "lexer.h"


enum class ASTNodeType : uint32_t {
  EMPTY = 0,
  MUL = 1,
  DIV = 2,
  ADD = 3,
  SUB = 4,
  NEG = 5,
  EQUAL = 6,
  NOT_EQUAL = 7,
  LESS_THAN = 8,
  LESS_EQUAL = 9,
  INTEGER = 10,
  VARIABLE = 11,
  ASSIGN = 12,
  IF_ELSE = 13,
  COMPOUND_STATEMENT = 14
};

class ASTNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;

struct ASTNode {
  ASTNodeType type_;
  std::string literal_; 
  std::vector<ASTNodePtr> children_;

  ASTNode(ASTNodeType type, const std::string& literal) 
    : type_(type), literal_(literal) {}

  ASTNode(ASTNodeType type, std::vector<ASTNodePtr> childern) 
    : type_(type), children_(std::move(childern)) {}

  std::string to_string() const;
  bool operator==(const ASTNode&) const;
  bool operator!=(const ASTNode& other) const { return !(*this == other); }

  static ASTNodePtr empty();
  static ASTNodePtr multiply(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr divide(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr add(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr subtract(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr negtive(ASTNodePtr operand);
  static ASTNodePtr equal(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr not_equal(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr less_than(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr less_equal(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr integer(const std::string& literal);
  static ASTNodePtr variable(const std::string& name);
  static ASTNodePtr assign(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr if_else(ASTNodePtr expression, ASTNodePtr if_statement, ASTNodePtr else_statement);
  static ASTNodePtr compound_statement(std::vector<ASTNodePtr> children);
};

