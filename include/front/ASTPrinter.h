#include "front/AST.h"
#include <iostream>

namespace front {
class Indent {
  uint32_t indent_ = 0;
  std::ostream &os_;

public:
  Indent(std::ostream &os) : os_(os) {}
  Indent &operator++() {
    indent_++;
    return *this;
  }
  Indent &operator--() {
    indent_--;
    return *this;
  }
  friend std::ostream &operator<<(std::ostream &os, const Indent &indent) {
    for (int i = 0; i < indent.indent_; i++) {
      os << "  ";
    }
    return os;
  }
};

class ASTPrintVisitor : public ASTVisitor {
public:
  ASTPrintVisitor(std::ostream &os = std::cerr) : indent_(os), os_(os) {}

  void visit(const EmptyNode &node);
  void visit(const BinaryOperationNode &node);
  void visit(const UnaryOperationNode &node);
  void visit(const LiteralNode &node);
  void visit(const IfElseNode &node);
  void visit(const ForLoopNode &node);
  void visit(const DoWhileNode &node);
  void visit(const WhileLoopNode &node);
  void visit(const CompoundStatementNode &node);
  void visit(const VariableDeclarationNode &node);
  void visit(const FunctionDeclarationNode &node);
  void visit(const DeclarationStatementNode &node);
  void visit(const ReturnNode &node);

protected:
  Indent indent_;
  std::ostream &os_;
};

} // namespace front