#include "front/AST.h"
#include <map>
#include <optional>
#include <stdexcept>

namespace front {
ASTNodePtr ASTNode::empty() { return std::make_shared<EmptyNode>(); }

BinaryOperationNodePtr ASTNode::Multiply(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::MUL);
}

BinaryOperationNodePtr ASTNode::Divide(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::DIV);
}

BinaryOperationNodePtr ASTNode::Add(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::ADD);
}

BinaryOperationNodePtr ASTNode::Subtract(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::SUB);
}

BinaryOperationNodePtr ASTNode::Equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::EQUAL);
}

BinaryOperationNodePtr ASTNode::Not_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::NOT_EQUAL);
}

BinaryOperationNodePtr ASTNode::Less_than(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::LESS_THAN);
}

BinaryOperationNodePtr ASTNode::Less_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::LESS_EQUAL);
}

BinaryOperationNodePtr ASTNode::Assign(ASTNodePtr lhs, ASTNodePtr rhs) {
  return std::make_shared<BinaryOperationNode>(
      std::move(lhs), std::move(rhs), ASTNodeKind::ASSIGN);
}

LiteralNodePtr ASTNode::Integer(const std::string& literal) {
  return std::make_shared<LiteralNode>(literal, ASTNodeKind::INTEGER);
}

LiteralNodePtr ASTNode::Identifier(const std::string& name) {
  return std::make_shared<LiteralNode>(name, ASTNodeKind::IDENTIFIER);
}

UnaryOperationNodePtr ASTNode::Negtive(ASTNodePtr operand) {
  return std::make_shared<UnaryOperationNode>(std::move(operand),
                                              ASTNodeKind::NEG);
}

IfElseNodePtr ASTNode::If_else(ASTNodePtr expression,
                               ASTNodePtr if_statement,
                               ASTNodePtr else_statement) {
  return std::make_shared<IfElseNode>(std::move(expression),
                                      std::move(if_statement),
                                      std::move(else_statement));
}

ForLoopNodePtr ASTNode::For_loop(ASTNodePtr init_expression,
                                 ASTNodePtr condition_expression,
                                 ASTNodePtr increment_expression,
                                 ASTNodePtr statement) {
  return std::make_shared<ForLoopNode>(std::move(init_expression),
                                       std::move(condition_expression),
                                       std::move(increment_expression),
                                       std::move(statement));
}

DoWhileNodePtr ASTNode::Do_while(ASTNodePtr statement,
                                 ASTNodePtr condition_expression) {
  return std::make_shared<DoWhileNode>(std::move(statement),
                                       std::move(condition_expression));
}

WhileLoopNodePtr ASTNode::While_loop(ASTNodePtr condition_expression,
                                     ASTNodePtr statement) {
  return std::make_shared<WhileLoopNode>(std::move(condition_expression),
                                         std::move(statement));
}

CompoundStatementNodePtr
ASTNode::Compound_statement(std::vector<ASTNodePtr> children) {
  return std::make_shared<CompoundStatementNode>(std::move(children));
}

VariableDeclarationNodePtr
ASTNode::Variable_declaration(DeclaratorPtr declarator,
                              ASTNodePtr    initializer) {
  return std::make_shared<VariableDeclarationNode>(std::move(declarator),
                                                   std::move(initializer));
}

DeclarationStatementNodePtr ASTNode::Declaration_statement(
    std::vector<DeclarationNodePtr> declaration_list) {
  return std::make_shared<DeclarationStatementNode>(
      std::move(declaration_list));
}

FunctionDeclarationNodePtr
ASTNode::Function_declaration(DeclaratorPtr            declarator,
                              CompoundStatementNodePtr body) {
  return std::make_shared<FunctionDeclarationNode>(std::move(declarator),
                                                   std::move(body));
}

FunctionDeclarationNodePtr ASTNode::Function_declaration(
    DeclaratorPtr                            declarator,
    std::vector<DeclarationStatementNodePtr> declaration_statement_list,
    CompoundStatementNodePtr                 body) {
  return std::make_shared<FunctionDeclarationNode>(
      std::move(declarator),
      std::move(declaration_statement_list),
      std::move(body));
}

DeclarationNodePtr ASTNode::Normalize_declaration(ASTNodePtr node) {
  auto&       declaration_node = static_cast<DeclarationNode&>(*node);
  auto&       declarator       = declaration_node.declarator_;
  const auto& type             = declarator->type_;
  if(type == nullptr) {
    throw std::invalid_argument("Expect type for a declaration");
  }
  if(type->kind_ == TypeKind::FUNCTION) {
    return Normalize_function_declaration(node);
  }
  return ASTNode::Normalize_variable_declaration(node);
}

VariableDeclarationNodePtr
ASTNode::Normalize_variable_declaration(ASTNodePtr node) {
  auto& variable_node = static_cast<VariableDeclarationNode&>(*node);
  auto& declarator    = variable_node.declarator_;
  auto& initializer   = variable_node.initializer_;
  declarator->type_   = Type::NormalizeVariableDeclaration(declarator->type_);
  return ASTNode::Variable_declaration(std::move(declarator),
                                       std::move(initializer));
}

FunctionDeclarationNodePtr
ASTNode::Normalize_function_declaration(ASTNodePtr node) {
  auto& function_node = static_cast<FunctionDeclarationNode&>(*node);
  auto& declarator    = function_node.declarator_;
  auto& declaration_statement_list = function_node.declaration_statement_list_;
  auto& body                       = function_node.body_;

  if(declarator->type_->kind_ != TypeKind::FUNCTION) {
    throw std::invalid_argument("Creating a declarator on a type not function");
  }
  auto& function_type = static_cast<FunctionType&>(*declarator->type_);
  // If the function dont have a return type, set 'INT' to it
  if(function_type.return_type_ == nullptr) {
    function_type.return_type_ = Type::DefaultType();
  }

  auto return_type = function_type.return_type_;
  if(return_type->kind_ == TypeKind::FUNCTION) {
    throw std::invalid_argument("Function cannot return fucntion type");
  } else if(return_type->kind_ == TypeKind::ARRAY) {
    throw std::invalid_argument("Function cannot return array type");
  }

  std::optional<bool>         is_parameter_typed;
  std::vector<DeclaratorPtr>& parameters = function_type.parameter_types_;
  for(const DeclaratorPtr& parameter: parameters) {
    bool typed = (parameter->type_ != nullptr);
    if(is_parameter_typed.has_value()) {
      // all parameter should be either typed or not typed
      if(typed != is_parameter_typed.value()) {
        throw std::invalid_argument("Some parameter has type but some not");
      }
    } else {
      // Init
      is_parameter_typed = typed;
    }
  }

  std::vector<DeclarationNodePtr> declaration_list;
  for(const auto& declaration_statement: declaration_statement_list) {
    for(const auto& declaration: declaration_statement->declaration_list_) {
      declaration_list.push_back(declaration);
    }
  }

  // if parameter are already typed
  if(is_parameter_typed.value_or(false)) {
    if(!declaration_list.empty()) {
      throw std::invalid_argument("old-style parameter declarations in "
                                  "prototyped function definition");
    }
  } else {
    // Get a map from name to pointer of declarator
    std::map<std::string, Declarator*> name_to_parameter;
    for(DeclaratorPtr& parameter: parameters) {
      const auto& identifier = parameter->identifier_;
      if(!identifier.empty()) {
        auto insert_result
            = name_to_parameter.insert({ identifier, parameter.get() });
        if(!insert_result.second) {
          throw std::invalid_argument("Redefinition of parameter:"
                                      + identifier);
        }
      }
    }

    // Fill type to parameters
    for(auto& parameter_declaration: declaration_list) {
      auto& declaration_node
          = static_cast<DeclarationNode&>(*parameter_declaration);
      auto& declarator_in_node = declaration_node.declarator_;
      if(declarator_in_node->type_ == nullptr) {
        throw std::invalid_argument(
            "Declaration in declaration list should be typed");
      }
      if(declarator_in_node->identifier_.empty()) {
        throw std::invalid_argument("Declaration does not declare a parameter");
      }
      auto aim_iter = name_to_parameter.find(declarator_in_node->identifier_);
      if(aim_iter == name_to_parameter.end()) {
        throw std::invalid_argument("Parameter named '"
                                    + declarator_in_node->identifier_
                                    + "' is missing");
      }
      aim_iter->second->type_ = declarator_in_node->type_;
    }

    // Fill the default type 'INT' to untyped parameters
    for(auto& name_to_parameter_pair: name_to_parameter) {
      if(name_to_parameter_pair.second->type_ == nullptr) {
        name_to_parameter_pair.second->type_ = Type::DefaultType();
      }
    }
  }

  declarator->type_ = Type::NormalizeParameterDeclaration(declarator->type_);
  return std::make_shared<FunctionDeclarationNode>(std::move(declarator),
                                                   std::move(body));
}

DeclarationNodePtr ASTNode::Declaration(DeclaratorPtr declarator,
                                        ASTNodePtr    initializer) {
  const auto& type = declarator->type_;
  if(type == nullptr) {
    throw std::invalid_argument("Expect type for a declaration");
  }
  if(type->kind_ == TypeKind::FUNCTION) {
    if(initializer != nullptr) {
      throw std::invalid_argument(
          "illegal initializer (only variables can be initialized)");
    }
    return ASTNode::Function_declaration(std::move(declarator), {}, nullptr);
  }
  return ASTNode::Variable_declaration(std::move(declarator),
                                       std::move(initializer));
}

GetAddressNodePtr ASTNode::Get_address(ASTNodePtr node) {
  return std::make_shared<GetAddressNode>(std::move(node));
}

ReturnNodePtr ASTNode::Return(ASTNodePtr operand) {
  return std::make_shared<ReturnNode>(std::move(operand));
}

bool ASTNode::operator==(const ASTNode& other) const {
  return kind_ == other.kind_;
}

bool EmptyNode::operator==(const ASTNode& other) const {
  return kind_ == other.kind_;
}

bool BinaryOperationNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const BinaryOperationNode&>(other);
  return *lhs_ == *typed_other.lhs_ && *rhs_ == *typed_other.rhs_;
}

bool UnaryOperationNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const UnaryOperationNode&>(other);
  return *operand_ == *typed_other.operand_;
}

bool LiteralNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const LiteralNode&>(other);
  return literal_ == typed_other.literal_;
}

bool IfElseNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const IfElseNode&>(other);
  return *expression_ == *typed_other.expression_
         && *if_statement_ == *typed_other.if_statement_
         && *else_statement_ == *typed_other.else_statement_;
}

bool ForLoopNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const ForLoopNode&>(other);
  return *init_expression_ == *typed_other.init_expression_
         && *condition_expression_ == *typed_other.condition_expression_
         && *increment_expression_ == *typed_other.increment_expression_
         && *statement_ == *typed_other.statement_;
}

bool DoWhileNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const DoWhileNode&>(other);
  return *statement_ == *typed_other.statement_
         && *condition_expression_ == *typed_other.condition_expression_;
}

bool WhileLoopNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const WhileLoopNode&>(other);
  return *condition_expression_ == *typed_other.condition_expression_
         && *statement_ == *typed_other.statement_;
}

bool CompoundStatementNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const CompoundStatementNode&>(other);
  if(children_.size() != typed_other.children_.size()) {
    return false;
  }
  for(size_t i = 0; i < children_.size(); ++i) {
    if(*children_[i] != *typed_other.children_[i]) {
      return false;
    }
  }
  return true;
}

bool VariableDeclarationNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const VariableDeclarationNode&>(other);
  if(*declarator_ != *typed_other.declarator_) return false;
  if((initializer_ == nullptr) != (typed_other.initializer_ == nullptr)) {
    return false;
  }
  if(initializer_ == nullptr) {
    return true;
  }
  return *initializer_ == *typed_other.initializer_;
}

bool FunctionDeclarationNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const FunctionDeclarationNode&>(other);
  if(*declarator_ != *typed_other.declarator_) {
    return false;
  }
  if(body_ == nullptr) {
    return typed_other.body_ == nullptr;
  } else {
    return typed_other.body_ != nullptr && *body_ == *typed_other.body_;
  }
  return true;
}

bool DeclarationStatementNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const DeclarationStatementNode&>(other);
  if(declaration_list_.size() != typed_other.declaration_list_.size()) {
    return false;
  }
  for(size_t i = 0; i < declaration_list_.size(); i++) {
    if(*declaration_list_[i] != *typed_other.declaration_list_[i]) {
      return false;
    }
  }
  return true;
}

bool GetAddressNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const GetAddressNode&>(other);
  return *operand_ == *typed_other.operand_;
}

bool ReturnNode::operator==(const ASTNode& other) const {
  if(kind_ != other.kind_) {
    return false;
  }
  auto typed_other = static_cast<const ReturnNode&>(other);
  return *result_ == *typed_other.result_;
}

}  // namespace front
