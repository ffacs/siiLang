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
  INTEGER = 6,
  VARIABLE = 7,
  STATEMENTS = 8
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
};

ASTNodePtr astNode(ASTNodeType type, TokenPtr token);
ASTNodePtr astNode(ASTNodeType type, TokenPtr token, std::vector<ASTNodePtr> children);

class AST {
  ASTNodePtr root;
};
