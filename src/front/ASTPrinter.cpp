#include "front/ASTPrinter.h"
#include <map>
#include <sstream>

namespace front {
void ASTPrintVisitor::visit(const ASTNode &node) {
  switch (node.kind_) {
  case ASTNodeKind::EMPTY: {
    os_ << indent_ << "EmptyNode: "
        << "\n";
    return;
  }
  case ASTNodeKind::MUL:
  case ASTNodeKind::DIV:
  case ASTNodeKind::ADD:
  case ASTNodeKind::SUB:
  case ASTNodeKind::EQUAL:
  case ASTNodeKind::NOT_EQUAL:
  case ASTNodeKind::LESS_THAN:
  case ASTNodeKind::LESS_EQUAL:
  case ASTNodeKind::ASSIGN: {
    const BinaryOperationNode &binary_operation_node =
        static_cast<const BinaryOperationNode &>(node);
    return visit(binary_operation_node);
  }
  case ASTNodeKind::NEG: {
    const UnaryOperationNode &unary_operation_node =
        static_cast<const UnaryOperationNode &>(node);
    return visit(unary_operation_node);
  }
  case ASTNodeKind::INTEGER:
  case ASTNodeKind::IDENTIFIER: {
    const LiteralNode &literal_node = static_cast<const LiteralNode &>(node);
    return visit(literal_node);
  }
  case ASTNodeKind::COMPOUND_STATEMENT: {
    const CompoundStatementNode &compound_statement_node =
        static_cast<const CompoundStatementNode &>(node);
    return visit(compound_statement_node);
  }
  case ASTNodeKind::IF_ELSE: {
    const IfElseNode &if_else_node = static_cast<const IfElseNode &>(node);
    return visit(if_else_node);
  }
  case ASTNodeKind::FOR_LOOP: {
    const ForLoopNode &for_loop_node = static_cast<const ForLoopNode &>(node);
    return visit(for_loop_node);
  }
  case ASTNodeKind::DO_WHILE: {
    const DoWhileNode &do_while_node = static_cast<const DoWhileNode &>(node);
    return visit(do_while_node);
  }
  case ASTNodeKind::WHILE_LOOP: {
    const WhileLoopNode &while_loop_node =
        static_cast<const WhileLoopNode &>(node);
    return visit(while_loop_node);
  }
  case ASTNodeKind::VARIABLE_DECLARATION: {
    const VariableDeclarationNode &variable_declaration_node =
        static_cast<const VariableDeclarationNode &>(node);
    return visit(variable_declaration_node);
  }
  case ASTNodeKind::FUNCTION_DECLARATION: {
    const FunctionDeclarationNode &function_declaration_node =
        static_cast<const FunctionDeclarationNode &>(node);
    return visit(function_declaration_node);
  }
  case ASTNodeKind::DECLARATION_STATEMENT: {
    const DeclarationStatementNode &declaration_statement_node =
        static_cast<const DeclarationStatementNode &>(node);
    return visit(declaration_statement_node);
  }
  default:
    std::stringstream error_msg;
    error_msg << "Unknow type of AST Node" << static_cast<uint32_t>(node.kind_);
    throw std::invalid_argument(error_msg.str());
  }
}

static std::map<ASTNodeKind, std::string> BINARY_OPERATION_TYPE_TO_STRING = {
    {ASTNodeKind::MUL, "*"},       {ASTNodeKind::DIV, "/"},
    {ASTNodeKind::ADD, "+"},       {ASTNodeKind::SUB, "-"},
    {ASTNodeKind::EQUAL, "=="},    {ASTNodeKind::NOT_EQUAL, "!="},
    {ASTNodeKind::LESS_THAN, "<"}, {ASTNodeKind::LESS_EQUAL, "<="},
};

void ASTPrintVisitor::visit(const BinaryOperationNode &node) {
  switch (node.kind_) {
  case ASTNodeKind::MUL:
  case ASTNodeKind::DIV:
  case ASTNodeKind::ADD:
  case ASTNodeKind::SUB:
  case ASTNodeKind::EQUAL:
  case ASTNodeKind::NOT_EQUAL:
  case ASTNodeKind::LESS_THAN:
  case ASTNodeKind::LESS_EQUAL:
  case ASTNodeKind::ASSIGN: {
    os_ << indent_ << "BinaryOperationNode: "
        << BINARY_OPERATION_TYPE_TO_STRING[node.kind_] << "\n";
    ++indent_;
    visit(*node.lhs_);
    visit(*node.rhs_);
    --indent_;
    return;
  }
  default: {
    std::stringstream error_msg;
    error_msg << "Unknow type of BinaryOperationNode"
              << static_cast<uint32_t>(node.kind_);
    throw std::invalid_argument(error_msg.str());
  }
  }
}

void ASTPrintVisitor::visit(const UnaryOperationNode &node) {
  switch (node.kind_) {
  case ASTNodeKind::NEG: {
    os_ << indent_ << "UnaryOperationNode: -"
        << "\n";
    ++indent_;
    visit(*node.operand_);
    --indent_;
    return;
  }
  default: {
    std::stringstream error_msg;
    error_msg << "Unknow type of UnaryOperationNode"
              << static_cast<uint32_t>(node.kind_);
    throw std::invalid_argument(error_msg.str());
  }
  }
}

void ASTPrintVisitor::visit(const LiteralNode &node) {
  switch (node.kind_) {
  case ASTNodeKind::INTEGER: {
    os_ << indent_ << "LiteralNode: "
        << " int " << node.literal_ << "\n";
    return;
  }
  case ASTNodeKind::IDENTIFIER: {
    os_ << indent_ << "LiteralNode: "
        << " identifier " << node.literal_ << "\n";
    return;
  }
  default: {
    std::stringstream error_msg;
    error_msg << "Unknow type of LiteralNode"
              << static_cast<uint32_t>(node.kind_);
    throw std::invalid_argument(error_msg.str());
  }
  }
}

void ASTPrintVisitor::visit(const IfElseNode &node) {
  os_ << indent_ << "IfElseNode: "
      << "\n";
  ++indent_;
  visit(*node.expression_);
  visit(*node.if_statement_);
  if (node.else_statement_) {
    visit(*node.else_statement_);
  }
  --indent_;
}

void ASTPrintVisitor::visit(const ForLoopNode &node) {
  os_ << indent_ << "ForLoopNode: "
      << "\n";
  ++indent_;
  visit(*node.init_expression_);
  visit(*node.condition_expression_);
  visit(*node.increment_expression_);
  visit(*node.statement_);
  --indent_;
}

void ASTPrintVisitor::visit(const DoWhileNode &node) {
  os_ << indent_ << "DoWhileNode: "
      << "\n";
  ++indent_;
  visit(*node.statement_);
  visit(*node.condition_expression_);
  --indent_;
}

void ASTPrintVisitor::visit(const WhileLoopNode &node) {
  os_ << indent_ << "WhileLoopNode: "
      << "\n";
  ++indent_;
  visit(*node.condition_expression_);
  visit(*node.statement_);
  --indent_;
}

void ASTPrintVisitor::visit(const CompoundStatementNode &node) {
  os_ << indent_ << "CompoundStatementNode: "
      << "\n";
  ++indent_;
  for (const auto &child : node.children_) {
    visit(*child);
  }
  --indent_;
}

void ASTPrintVisitor::visit(const VariableDeclarationNode &node) {
  os_ << indent_ << "VariableDeclarationNode: "
      << "\n";
  ++indent_;
  os_ << indent_ << "Declarator: " << node.declarator_->to_string() << "\n";
  if (node.initializer_) {
    visit(*node.initializer_);
  }
  --indent_;
}

void ASTPrintVisitor::visit(const FunctionDeclarationNode &node) {
  os_ << indent_ << "FunctionDeclarationNode: "
      << "\n";
  ++indent_;
  os_ << indent_ << "Declarator: " << node.declarator_->to_string() << "\n";
  for (const auto &child : node.declaration_statement_list_) {
    visit(*child);
  }
  if (node.body_) {
    visit(*node.body_);
  }
  --indent_;
}

void ASTPrintVisitor::visit(const DeclarationStatementNode &node) {
  os_ << indent_ << "DeclarationStatementNode: "
      << "\n";
  ++indent_;
  for (const auto &child : node.declaration_list_) {
    visit(*child);
  }
  --indent_;
}

} // namespace front
