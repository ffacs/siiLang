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
  IDENTIFIER = 11,
  ASSIGN = 12,
  IF_ELSE = 13,
  FOR_LOOP = 14,
  DO_WHILE = 15,
  WHILE_LOOP = 16,
  COMPOUND_STATEMENT = 17,
  VARIABLE_DECLARATION = 18,
  DECLARATION = 19,
  DECLARATION_LIST = 20,
  FUNCTION_DECLARATION = 21,
};

class ASTNode;
class VariableDeclarationNode;
class DeclarationNode;
class DeclarationStatementNode;
class FunctionDeclarationNode;
typedef std::shared_ptr<ASTNode> ASTNodePtr;
typedef std::shared_ptr<DeclarationNode> DeclarationNodePtr;
typedef std::shared_ptr<VariableDeclarationNode> VariableDeclarationNodePtr;
typedef std::shared_ptr<FunctionDeclarationNode> FunctionDeclarationNodePtr;
typedef std::shared_ptr<DeclarationStatementNode> DeclarationStatementNodePtr;

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
  static ASTNodePtr identifier(const std::string& name);
  static ASTNodePtr assign(ASTNodePtr lhs, ASTNodePtr rhs);
  static ASTNodePtr if_else(ASTNodePtr expression, ASTNodePtr if_statement, ASTNodePtr else_statement);
  static ASTNodePtr for_loop(ASTNodePtr init_expression, ASTNodePtr condition_expression, ASTNodePtr add_expression, ASTNodePtr statement);
  static ASTNodePtr do_while(ASTNodePtr statement, ASTNodePtr condition_expression);
  static ASTNodePtr while_loop(ASTNodePtr condition_expression, ASTNodePtr statement);
  static ASTNodePtr compound_statement(std::vector<ASTNodePtr> children);
  static VariableDeclarationNodePtr variable_declaration(DeclaratorPtr declarator, ASTNodePtr initializer);
  static FunctionDeclarationNodePtr function_declaration(DeclaratorPtr declarator, ASTNodePtr body);
  static FunctionDeclarationNodePtr function_declaration(DeclaratorPtr declarator,
                                                         std::vector<DeclarationStatementNodePtr> declaration_statement_list,
                                                         ASTNodePtr body);
  static DeclarationNodePtr declaration(DeclaratorPtr declarator, ASTNodePtr initializer);
  static VariableDeclarationNodePtr normalize_variable_declaration(ASTNodePtr node);
  static FunctionDeclarationNodePtr normalize_function_declaration(ASTNodePtr node);
  static DeclarationNodePtr         normalize_declaration(ASTNodePtr node);
  static DeclarationStatementNodePtr declaration_statement(std::vector<DeclarationNodePtr> declaration_list);
};

struct DeclarationNode : public ASTNode {
  DeclaratorPtr declarator_;
  DeclarationNode(ASTNodeKind kind, DeclaratorPtr declarator)
    : ASTNode(kind), declarator_(std::move(declarator)) {}

  std::string to_string() const override;
  bool operator==(const ASTNode&) const override;
};

struct VariableDeclarationNode : public DeclarationNode {
  ASTNodePtr initializer_;

  VariableDeclarationNode(DeclaratorPtr declarator, ASTNodePtr initializer) :
    DeclarationNode(ASTNodeKind::VARIABLE_DECLARATION, std::move(declarator)),
    initializer_(std::move(initializer)) {}

  std::string to_string() const override;
  bool operator==(const ASTNode&) const override;
};

struct FunctionDeclarationNode : public DeclarationNode {
  ASTNodePtr body_;
  std::vector<DeclarationStatementNodePtr> declaration_statement_list_;


  FunctionDeclarationNode(DeclaratorPtr declarator, 
                          std::vector<DeclarationStatementNodePtr> declaration_statement_list,
                          ASTNodePtr body)
    : DeclarationNode(ASTNodeKind::FUNCTION_DECLARATION, std::move(declarator)),
      declaration_statement_list_(std::move(declaration_statement_list)),
      body_(std::move(body)) {}

  FunctionDeclarationNode(DeclaratorPtr declarator, ASTNodePtr body)
    : DeclarationNode(ASTNodeKind::FUNCTION_DECLARATION, std::move(declarator)),
      body_(std::move(body)) {}

  std::string to_string() const override;
  bool operator==(const ASTNode&) const override;
};

struct DeclarationStatementNode : public ASTNode {
  std::vector<DeclarationNodePtr> declaration_list_;
  
  DeclarationStatementNode(std::vector<DeclarationNodePtr> declaration_list) :
    ASTNode(ASTNodeKind::DECLARATION),
    declaration_list_(std::move(declaration_list)) {}

  std::string to_string() const override;
  bool operator==(const ASTNode&) const override;
};
