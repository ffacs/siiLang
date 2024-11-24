#include "AST.h"
#include <sstream>
#include <map>

static inline ASTNodePtr astNode(ASTNodeType type, const std::string& literal) {
  return std::make_shared<ASTNode>(type, literal);
}

static ASTNodePtr astNode(ASTNodeType type, std::vector<ASTNodePtr> children) {
  return std::make_shared<ASTNode>(type, std::move(children));
}

ASTNodePtr ASTNode::empty() {
  return astNode(ASTNodeType::EMPTY, "");
}

ASTNodePtr ASTNode::multiply(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::MUL, {lhs, rhs});
}

ASTNodePtr ASTNode::divide(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::DIV, {lhs, rhs});
}

ASTNodePtr ASTNode::add(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::ADD, {lhs, rhs});
}

ASTNodePtr ASTNode::subtract(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::SUB, {lhs, rhs});
}

ASTNodePtr ASTNode::negtive(ASTNodePtr operand) {
  return astNode(ASTNodeType::NEG, { operand });
}

ASTNodePtr ASTNode::equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::EQUAL, {lhs, rhs});
}

ASTNodePtr ASTNode::not_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::NOT_EQUAL, {lhs, rhs});
}

ASTNodePtr ASTNode::less_than(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::LESS_THAN, {lhs, rhs});
}

ASTNodePtr ASTNode::less_equal(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::LESS_EQUAL, {lhs, rhs});
}

ASTNodePtr ASTNode::integer(const std::string& literal) {
  return astNode(ASTNodeType::INTEGER, literal);
}

ASTNodePtr ASTNode::variable(const std::string& name) {
  return astNode(ASTNodeType::VARIABLE, name);
}

ASTNodePtr ASTNode::assign(ASTNodePtr lhs, ASTNodePtr rhs) {
  return astNode(ASTNodeType::ASSIGN, {lhs, rhs});
}

ASTNodePtr ASTNode::if_else(ASTNodePtr expression, ASTNodePtr if_statement, ASTNodePtr else_statement) {
  return astNode(ASTNodeType::IF_ELSE, {expression, if_statement, else_statement});
}

ASTNodePtr ASTNode::compound_statement(std::vector<ASTNodePtr> children) {
  return astNode(ASTNodeType::COMPOUND_STATEMENT, std::move(children));
}

static std::map<ASTNodeType, std::string> BINARY_OPERATION_TYPE_TO_STRING = {
  {ASTNodeType::MUL, "*"},
  {ASTNodeType::DIV, "/"},
  {ASTNodeType::ADD, "+"},
  {ASTNodeType::SUB, "-"},
  {ASTNodeType::EQUAL, "=="},
  {ASTNodeType::NOT_EQUAL, "!="},
  {ASTNodeType::LESS_THAN, "<"},
  {ASTNodeType::LESS_EQUAL, "<="},
};

std::string ASTNode::to_string() const {
  std::stringstream ss;
  switch (type_) {
    case ASTNodeType::MUL:
    case ASTNodeType::DIV: 
    case ASTNodeType::ADD: 
    case ASTNodeType::SUB:
    case ASTNodeType::EQUAL: 
    case ASTNodeType::NOT_EQUAL: 
    case ASTNodeType::LESS_THAN: 
    case ASTNodeType::LESS_EQUAL: {
      ss << "(";
      ss << children_[0]->to_string();
      ss << BINARY_OPERATION_TYPE_TO_STRING[type_];
      ss << children_[1]->to_string();
      ss << ")";
      return ss.str();
    }
    case ASTNodeType::INTEGER: {
      ss << literal_;
      return ss.str();
    }
    case ASTNodeType::VARIABLE: {
      ss << literal_;
      return ss.str();
    } 
    case ASTNodeType::ASSIGN: {
      ss << children_[0]->to_string() << " = " << children_[1]->to_string();
      return ss.str();
    }
    case ASTNodeType::IF_ELSE: {
      ss << "if (" << children_[0]->to_string() << ") ";
      ss << children_[1]->to_string();
      if (children_[2] != nullptr) {
        ss << "else" << children_[2] ->to_string();
      }
      return ss.str();
    }
    case ASTNodeType::COMPOUND_STATEMENT: {
      ss << "{";
        for (const auto& node : children_) {
          ss << node->to_string() << ";";
        }
      ss << "}";
      return ss.str();
    } default: {
      std::stringstream error_msg;
      error_msg << "unknow type of ASTNode " << static_cast<uint32_t>(type_) << " found";
      throw std::invalid_argument(error_msg.str());
    }
  }
}

bool ASTNode::operator==(const ASTNode & other) const {
  if (type_ != other.type_) return false;
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
