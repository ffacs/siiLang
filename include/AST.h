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
  STATEMENTS = 12
};

class ASTNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;

struct ASTNode {
  ASTNodeType type_;
  TokenPtr token_; 
  std::vector<ASTNodePtr> children_;

  ASTNode(ASTNodeType type, TokenPtr token) 
    : type_(type), token_(std::move(token)) {}

  ASTNode(ASTNodeType type, TokenPtr token, std::vector<ASTNodePtr> childern) 
    : type_(type), token_(std::move(token)), children_(std::move(childern)) {}

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
  static ASTNodePtr integer(TokenPtr literal);
  static ASTNodePtr variable(TokenPtr literal);
  static ASTNodePtr statements(std::vector<ASTNodePtr> children);
};

class AST {
  ASTNodePtr root;
};

