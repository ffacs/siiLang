#pragma once
#include <string>
#include <memory>
#include <vector>
#include "lexer.h"
#include "type.h"


enum class ASTNodeKind : uint32_t {
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
  FOR_LOOP = 14,
  DO_WHILE = 15,
  WHILE_LOOP = 16,
  COMPOUND_STATEMENT = 17,
  SINGLE_DECLARATION = 18,
  DECLARATION = 19,
};

class ASTNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;

struct ASTNode {
  ASTNodeKind kind_;
  std::string literal_; 
  std::vector<ASTNodePtr> children_;

  ASTNode(ASTNodeKind kind) 
    : kind_(kind) {}

  ASTNode(ASTNodeKind kind, const std::string& literal) 
    : kind_(kind), literal_(literal) {}

  ASTNode(ASTNodeKind kind, std::vector<ASTNodePtr> childern) 
    : kind_(kind), children_(std::move(childern)) {}

  virtual std::string to_string() const;
  virtual bool operator==(const ASTNode&) const;
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
  static ASTNodePtr for_loop(ASTNodePtr init_expression, ASTNodePtr condition_expression, ASTNodePtr add_expression, ASTNodePtr statement);
  static ASTNodePtr do_while(ASTNodePtr statement, ASTNodePtr condition_expression);
  static ASTNodePtr while_loop(ASTNodePtr condition_expression, ASTNodePtr statement);
  static ASTNodePtr compound_statement(std::vector<ASTNodePtr> children);
  static ASTNodePtr single_declaration(TypePtr type, ASTNodePtr identifier, ASTNodePtr initializer);
  static ASTNodePtr declaration(std::vector<ASTNodePtr> single_declaration_list);
};

struct SingleDeclarationNode : public ASTNode {
  TypePtr type_;
  ASTNodePtr identifier_;
  ASTNodePtr initializer_;
  SingleDeclarationNode(TypePtr type, ASTNodePtr identifier, ASTNodePtr initializer) :
    ASTNode(ASTNodeKind::SINGLE_DECLARATION),
    type_(std::move(type)),
    identifier_(std::move(identifier)),
    initializer_(std::move(initializer)) {}
  std::string to_string() const override;
  bool operator==(const ASTNode&) const override;
};
