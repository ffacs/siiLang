#include "AST.h"
#include <sstream>
#include <map>
#include <optional>

static inline ASTNodePtr astNode(ASTNodeKind type, const std::string& literal) {
  return std::make_shared<ASTNode>(type, literal);
}

static ASTNodePtr astNode(ASTNodeKind type, std::vector<ASTNodePtr> children) {
  return std::make_shared<ASTNode>(type, std::move(children));
}

ASTNodePtr ASTNode::empty() {
  return astNode(ASTNodeKind::EMPTY, "");
}

ASTNodePtr ASTNode::multiply(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::MUL, {lhs, rhs});
}

ASTNodePtr ASTNode::divide(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::DIV, {lhs, rhs});
}

ASTNodePtr ASTNode::add(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::ADD, {lhs, rhs});
}

ASTNodePtr ASTNode::subtract(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::SUB, {lhs, rhs});
}

ASTNodePtr ASTNode::negtive(ASTNodePtr operand) {
  return astNode(ASTNodeKind::NEG, { operand });
}

ASTNodePtr ASTNode::equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::EQUAL, {lhs, rhs});
}

ASTNodePtr ASTNode::not_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::NOT_EQUAL, {lhs, rhs});
}

ASTNodePtr ASTNode::less_than(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::LESS_THAN, {lhs, rhs});
}

ASTNodePtr ASTNode::less_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::LESS_EQUAL, {lhs, rhs});
}

ASTNodePtr ASTNode::integer(const std::string& literal) {
  return astNode(ASTNodeKind::INTEGER, literal);
}

ASTNodePtr ASTNode::identifier(const std::string& name) {
  return astNode(ASTNodeKind::IDENTIFIER, name);
}

ASTNodePtr ASTNode::assign(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeKind::ASSIGN, {lhs, rhs});
}

ASTNodePtr ASTNode::if_else(ASTNodePtr expression, ASTNodePtr if_statement, ASTNodePtr else_statement) {
  return astNode(ASTNodeKind::IF_ELSE, {expression, if_statement, else_statement});
}

ASTNodePtr ASTNode::for_loop(ASTNodePtr init_expression,
                    ASTNodePtr condition_expression,
                    ASTNodePtr increment_expression,
                    ASTNodePtr statement) {
  return astNode(ASTNodeKind::FOR_LOOP, {init_expression,
                                         condition_expression,
                                         increment_expression,
                                         statement});
}

ASTNodePtr ASTNode::do_while(ASTNodePtr statement, ASTNodePtr condition_expression) {
  return astNode(ASTNodeKind::DO_WHILE, {statement, condition_expression});
}

ASTNodePtr ASTNode::while_loop(ASTNodePtr condition_expression, ASTNodePtr statement) {
  return astNode(ASTNodeKind::WHILE_LOOP, {condition_expression, statement});
}

VariableDeclarationNodePtr ASTNode::variable_declaration(DeclaratorPtr declarator, ASTNodePtr initializer) {
  return std::make_shared<VariableDeclarationNode>(std::move(declarator), std::move(initializer));
}

DeclarationStatementNodePtr ASTNode::declaration_statement(std::vector<DeclarationNodePtr> declaration_list) {
  return std::make_shared<DeclarationStatementNode>(std::move(declaration_list));
}

FunctionDeclarationNodePtr ASTNode::function_declaration(DeclaratorPtr declarator, ASTNodePtr body) {
  return std::make_shared<FunctionDeclarationNode>(std::move(declarator), std::move(body));
}

FunctionDeclarationNodePtr ASTNode::function_declaration(DeclaratorPtr declarator,
                                                        std::vector<DeclarationStatementNodePtr> declaration_statement_list,
                                                        ASTNodePtr body) {
  return std::make_shared<FunctionDeclarationNode>(std::move(declarator), std::move(declaration_statement_list), std::move(body));
}

DeclarationNodePtr ASTNode::normalize_declaration(ASTNodePtr node) {
  auto& declaration_node = static_cast<DeclarationNode&>(*node);
  auto& declarator = declaration_node.declarator_;
  const auto& type = declarator->type_;
  if (type == nullptr) {
    throw std::invalid_argument("Expect type for a declaration");
  }
  if (type->kind_ == TypeKind::FUNCTION) {
    return normalize_function_declaration(node);
  }
  return ASTNode::normalize_variable_declaration(node);
}

VariableDeclarationNodePtr ASTNode::normalize_variable_declaration(ASTNodePtr node) {
  auto& variable_node = static_cast<VariableDeclarationNode&>(*node);
  auto& declarator = variable_node.declarator_;
  auto& initializer = variable_node.initializer_;
  declarator->type_ = Type::NormalizeVariableDeclaration(declarator->type_);
  return ASTNode::variable_declaration(std::move(declarator), std::move(initializer));
}

FunctionDeclarationNodePtr ASTNode::normalize_function_declaration(ASTNodePtr node) {
  auto& function_node = static_cast<FunctionDeclarationNode&>(*node);
  auto& declarator = function_node.declarator_;
  auto& declaration_statement_list = function_node.declaration_statement_list_;
  auto& body = function_node.body_;

  if (declarator->type_->kind_ != TypeKind::FUNCTION) {
    throw std::invalid_argument("Creating a declarator on a type not function");
  }
  auto& function_type = static_cast<FunctionType&>(*declarator->type_);
  // If the function dont have a return type, set 'INT' to it
  if (function_type.return_type_ == nullptr) {
    function_type.return_type_ = Type::default_type();
  }

  auto return_type = function_type.return_type_;
  if (return_type->kind_ == TypeKind::FUNCTION) {
    throw std::invalid_argument("Function cannot return fucntion type");
  } else if (return_type->kind_ == TypeKind::ARRAY) {
    throw std::invalid_argument("Function cannot return array type");
  }

  std::optional<bool> is_parameter_typed;
  std::vector<DeclaratorPtr>& parameters = function_type.parameter_types_;
  for (const DeclaratorPtr& parameter : parameters) {
    bool typed = (parameter->type_ != nullptr);
    if (is_parameter_typed.has_value()) {
      // all parameter should be either typed or not typed
      if (typed != is_parameter_typed.value()) {
        throw std::invalid_argument("Some parameter has type but some not");
      }
    } else {
      // Init
      is_parameter_typed = typed;
    }
  }
  
  std::vector<DeclarationNodePtr> declaration_list;
  for (const auto& declaration_statement : declaration_statement_list) {
    for (const auto& declaration : declaration_statement->declaration_list_) {
      declaration_list.push_back(declaration);
    }
  }

  // if parameter are already typed
  if (is_parameter_typed.value_or(false)) {
    if (!declaration_list.empty()) {
      throw std::invalid_argument("old-style parameter declarations in prototyped function definition");
    }
  } else {
    // Get a map from name to pointer of declarator
    std::map<std::string, Declarator*> name_to_parameter;
    for (DeclaratorPtr& parameter : parameters) {
      const auto& identifier = parameter->identifier_;
      if (!identifier.empty()) {
        auto insert_result = name_to_parameter.insert({identifier, parameter.get()});
        if (!insert_result.second) {
          throw std::invalid_argument("Redefinition of parameter:" + identifier);
        }
      }
    }

    // Fill type to parameters
    for (auto& parameter_declaration : declaration_list) {
      auto& declaration_node = static_cast<DeclarationNode&>(*parameter_declaration);
      auto& declarator_in_node = declaration_node.declarator_;
      if (declarator_in_node->type_ == nullptr) {
        throw std::invalid_argument("Declaration in declaration list should be typed");
      }
      if (declarator_in_node->identifier_.empty()) {
        throw std::invalid_argument("Declaration does not declare a parameter");
      }
      auto aim_iter = name_to_parameter.find(declarator_in_node->identifier_);
      if (aim_iter == name_to_parameter.end()) {
        throw std::invalid_argument("Parameter named '" + declarator_in_node->identifier_  + "' is missing");
      }
      aim_iter->second->type_ = declarator_in_node->type_;
    }

    // Fill the default type 'INT' to untyped parameters
    for (auto& name_to_parameter_pair : name_to_parameter) {
      if (name_to_parameter_pair.second->type_ == nullptr) {
        name_to_parameter_pair.second->type_ = Type::default_type();
      }
    }
  }

  declarator->type_ = Type::NormalizeParameterDeclaration(declarator->type_);
  return std::make_shared<FunctionDeclarationNode>(std::move(declarator), std::move(body));
}

DeclarationNodePtr ASTNode::declaration(DeclaratorPtr declarator, ASTNodePtr initializer) {
  const auto& type = declarator->type_;
  if (type == nullptr) {
    throw std::invalid_argument("Expect type for a declaration");
  }
  if (type->kind_ == TypeKind::FUNCTION) {
    if (initializer != nullptr) {
      throw std::invalid_argument("illegal initializer (only variables can be initialized)");
    }
    return ASTNode::function_declaration(std::move(declarator), {}, nullptr);
  }
  return ASTNode::variable_declaration(std::move(declarator), std::move(initializer));
}

ASTNodePtr ASTNode::compound_statement(std::vector<ASTNodePtr> children) {
  return astNode(ASTNodeKind::COMPOUND_STATEMENT, std::move(children));
}

static std::map<ASTNodeKind, std::string> BINARY_OPERATION_TYPE_TO_STRING = {
  {ASTNodeKind::MUL, "*"},
  {ASTNodeKind::DIV, "/"},
  {ASTNodeKind::ADD, "+"},
  {ASTNodeKind::SUB, "-"},
  {ASTNodeKind::EQUAL, "=="},
  {ASTNodeKind::NOT_EQUAL, "!="},
  {ASTNodeKind::LESS_THAN, "<"},
  {ASTNodeKind::LESS_EQUAL, "<="},
};

std::string ASTNode::to_string() const {
  std::stringstream ss;
  switch (kind_) {
    case ASTNodeKind::MUL:
    case ASTNodeKind::DIV: 
    case ASTNodeKind::ADD: 
    case ASTNodeKind::SUB:
    case ASTNodeKind::EQUAL: 
    case ASTNodeKind::NOT_EQUAL: 
    case ASTNodeKind::LESS_THAN: 
    case ASTNodeKind::LESS_EQUAL: {
      ss << "(";
      ss << children_[0]->to_string();
      ss << BINARY_OPERATION_TYPE_TO_STRING[kind_];
      ss << children_[1]->to_string();
      ss << ")";
      return ss.str();
    }
    case ASTNodeKind::INTEGER: {
      ss << literal_;
      return ss.str();
    }
    case ASTNodeKind::IDENTIFIER: {
      ss << literal_;
      return ss.str();
    } 
    case ASTNodeKind::ASSIGN: {
      ss << children_[0]->to_string() << " = " << children_[1]->to_string();
      return ss.str();
    }
    case ASTNodeKind::IF_ELSE: {
      ss << "if (" << children_[0]->to_string() << ") ";
      ss << children_[1]->to_string();
      if (children_[2] != nullptr) {
        ss << "else" << children_[2] ->to_string();
      }
      return ss.str();
    }
    case ASTNodeKind::COMPOUND_STATEMENT: {
      ss << "{\n";
        for (const auto& node : children_) {
          ss << node->to_string() << ";\n";
        }
      ss << "}\n";
      return ss.str();
    } default: {
      std::stringstream error_msg;
      error_msg << "unknow type of ASTNode " << static_cast<uint32_t>(kind_) << " found";
      throw std::invalid_argument(error_msg.str());
    }
  }
}

bool ASTNode::operator==(const ASTNode & other) const {
  if (kind_ != other.kind_) return false;
  if (children_.size() != other.children_.size()) return false;
  for (size_t i = 0; i < children_.size(); i++) {
    if ((children_[i] == nullptr) != (other.children_[i] == nullptr)) {
      return false;
    }
    if (children_[i] == nullptr) { continue; }
    if (*children_[i] != *other.children_[i]) {
      return false;
    }
  }
  return true;
}

bool DeclarationNode::operator==(const ASTNode &other) const {
  if (kind_ != other.kind_) { return false; }
  auto typed_other = static_cast<const DeclarationNode&>(other);
  return *declarator_ == *typed_other.declarator_;
}

std::string DeclarationNode::to_string() const {
  return declarator_->to_string();
}

bool VariableDeclarationNode::operator==(const ASTNode &other) const {
  if (kind_ != other.kind_) { return false; }
  auto typed_other = static_cast<const VariableDeclarationNode&>(other);
  if (*declarator_ != *typed_other.declarator_) return false;
  if ((initializer_ == nullptr) != (typed_other.initializer_ == nullptr)) { return false; }
  if (initializer_ == nullptr) { return true; }
  return *initializer_ == *typed_other.initializer_;
}

std::string VariableDeclarationNode::to_string() const {
    auto result = declarator_->to_string();
    if (initializer_ != nullptr) {
      result = result + " = " + initializer_->to_string();
    }
    return result;
};

bool FunctionDeclarationNode::operator==(const ASTNode &other) const {
  if (kind_ != other.kind_) { return false; }
  auto typed_other = static_cast<const FunctionDeclarationNode&>(other);
  if (*declarator_ != *typed_other.declarator_) { return false; }
  if (body_ == nullptr) {
    return typed_other.body_ == nullptr;
  } else {
    return typed_other.body_ != nullptr && *body_ == *typed_other.body_;
  }
  return true;
}

std::string FunctionDeclarationNode::to_string() const {
  std::stringstream ss;
  ss << declarator_->to_string();
  if (body_ != nullptr) {
    ss << body_->to_string();
  }
  return ss.str();
};

bool DeclarationStatementNode::operator==(const ASTNode &other) const {
  if (kind_ != other.kind_) { return false; }
  auto typed_other = static_cast<const DeclarationStatementNode&>(other);
  if (declaration_list_.size() != typed_other.declaration_list_.size()) {
    return false;
  }
  for (size_t i = 0; i < declaration_list_.size(); i++) {
    if (*declaration_list_[i] != *typed_other.declaration_list_[i]) {
      return false;
    }
  }
  return true;
}

std::string DeclarationStatementNode::to_string() const {
  std::stringstream ss;
  for (size_t i = 0; i < declaration_list_.size(); i++) {
    ss << declaration_list_[i]->to_string();
    if (i + 1 != children_.size()) {
      ss << ";";
    }
  }
  return ss.str();
};

