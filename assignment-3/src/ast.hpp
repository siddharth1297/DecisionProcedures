#ifndef __AST_HPP__
#define __AST_HPP__

#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#define NOT_IMPLEMENTED                                                        \
  std::cerr << "Not Implemented " << __FILE__ << ":" << __LINE__ << std::endl; \
  abort();
using namespace std;

typedef enum node_type {
  node_type_not,
  node_type_and,
  node_type_or,
  node_type_implication,
  node_type_bi_implication,
  node_type_equal,
  node_type_not_equal,
  node_type_var,
  node_type_func
} node_type_t;

static string opToStr[] = {"!", "&",  "|",   "->",  "<->",
                           "=", "!=", "VAR", "FUNC"};

static inline int isNodeTypeOp(node_type_t node_type) {
  return node_type >= node_type_not && node_type <= node_type_not_equal;
}

class Node {
public:
  Node() = delete;
  Node(node_type_t type, vector<Node *> &children)
      : type_(type), children_(children) {
    assert(isNodeTypeOp(type_));
  }
  Node(string &var_name) : type_(node_type_var), var_name_(var_name) {}
  Node(string &func_name, vector<Node *> &args, bool is_pred = false)
      : type_(node_type_func), func_name_(func_name), is_pred_(is_pred),
        children_(args) {}

  ~Node() {
    for (auto &child : children_) {
      delete child;
    }
  }
  friend ostream &operator<<(ostream &os, const Node *node) {
    if (node == nullptr) {
      os << "{}";
    } else {
      os << *node;
    }
    return os;
  }
  friend ostream &operator<<(ostream &os, const Node &node) {
    os << "{" << node.ToStr() << "}";
    return os;
  }

  std::string ToStr() const {
    if (type_ == node_type_func) {
      return func_name_;
    }
    if (type_ == node_type_var) {
      return var_name_;
    }
    if (isNodeTypeOp(type_)) {
      return opToStr[type_];
    }

    NOT_IMPLEMENTED;
  }

  std::string ToFormula() const {
    std::stringstream ss;
    ToFormula(ss);
    return ss.str();
  }

  void ToFormula(std::stringstream &ss) const {
    if (type_ == node_type_var) {
      ss << ToStr();
      assert(children_.empty());
      return;
    }

    if (type_ == node_type_func) {
      ss << ToStr();
      assert(!children_.empty());
      ss << "(";
      for (size_t i = 0; i < children_.size(); i++) {
        children_[i]->ToFormula(ss);
        if (i + 1 < children_.size()) {
          ss << ",";
        }
      }
      ss << ")";
      return;
    }
    assert(isNodeTypeOp(type_));
    if (type_ == node_type_not) {
      assert(children_.size() == 1);
      ss << ToStr();
      children_[0]->ToFormula(ss);
      return;
    }
    if (type_ != node_type_not) {
      assert(children_.size() == 2);
      children_[0]->ToFormula(ss);
      ss << " " << ToStr() << " ";
      children_[1]->ToFormula(ss);
      return;
    }
    std::cerr << this << std::endl;
    NOT_IMPLEMENTED;
  }

  inline std::string GetName() const {
    assert(type_ == node_type_var || type_ == node_type_func);
    return type_ == node_type_var ? var_name_ : func_name_;
  }
  node_type_t type_; // Type of the node
  string func_name_; // name of the (sub)function, set only if it
                     // is an operator node
  string var_name_;  // name of the variable
  bool is_pred_ = false;
  vector<Node *> children_;
};

class Ast {
public:
  Ast() = delete;
  Ast(Node *root) : root_(root) {}
  ~Ast() { delete root_; }
  Node *root_ = nullptr;
  friend ostream &operator<<(ostream &os, const Ast &ast) {
    os << "--------------------------------------" << endl;
    queue<Node *> q;
    os << "["
       << "(" << ast.root_ << ")"
       << "]" << std::endl;
    q.push(ast.root_);
    while (!q.empty()) {
      os << "[";
      size_t len = q.size();
      while (len--) {
        Node *node = q.front();
        q.pop();
        os << "(";
        for (size_t i = 0; i < node->children_.size(); i++) {
          auto &nd = node->children_[i];
          assert(nd != nullptr);
          q.push(nd);
          os << nd;
          if (i + 1 != node->children_.size())
            os << " ";
        }
        os << ")";
        if (len > 0)
          os << " ";
      }
      os << "]" << endl;
    }
    os << "--------------------------------------" << endl;
    return os;
  }

  std::string ToFormula() {
    std::stringstream ss;
    root_->ToFormula(ss);
    return ss.str();
  }
};

#endif
