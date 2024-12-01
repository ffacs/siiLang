#include "AST.h"
#include <sstream>
#include <map>

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

ASTNodePtr ASTNode::variable(const std::string& name) {
  return astNode(ASTNodeKind::VARIABLE, name);
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

ASTNodePtr ASTNode::single_declaration(TypePtr type, ASTNodePtr identifier, ASTNodePtr initializer) {
  return std::make_shared<SingleDeclarationNode>(std::move(type), std::move(identifier), std::move(initializer));
}

ASTNodePtr ASTNode::declaration(std::vector<ASTNodePtr> single_declaration_list) {
  return astNode(ASTNodeKind::DECLARATION, std::move(single_declaration_list));
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
    case ASTNodeKind::VARIABLE: {
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
    case ASTNodeKind::DECLARATION: {
      for (const auto& single_declaration : children_) {
        ss << single_declaration->to_string() << ";\n";
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

bool SingleDeclarationNode::operator==(const ASTNode &other) const {
  if (kind_ != other.kind_) { return false; }
  auto typed_other = reinterpret_cast<const SingleDeclarationNode&>(other);
  if (*type_ != *typed_other.type_) return false;
  if ((initializer_ == nullptr) != (typed_other.initializer_ == nullptr)) { return false; }
  if (initializer_ == nullptr) { return true; }
  return *initializer_ == *typed_other.initializer_;
}

std::string SingleDeclarationNode::to_string() const {
    auto result = type_->to_string(identifier_->literal_);
    if (initializer_ != nullptr) {
      result = result + " = " + initializer_->to_string();
    }
    return result;
};
