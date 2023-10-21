#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <cassert>
#include <cstddef>
#include <istream>
#include <string>

#include "ast.hpp"
#include "fe_parser.tab.hh"
#include "fe_scanner.hpp"

namespace FE {

class Driver {
public:
  Driver() = default;

  virtual ~Driver();

  /**
   * parse - parse from a file
   * @param filename - valid string with input file
   */
  Ast *parse(const char *const filename);
  /**
   * parse - parse from a c++ input stream
   * @param is - std::istream&, valid input stream
   */
  Ast *parse(std::istream &iss);

  std::ostream &print(std::ostream &stream);

  Node *updateRoot(Node *node) { return root_ = node; }

  Node *onNewVar(std::string &name) { return new Node(name); }
  Node *onNewFunction(std::string &funcName, vector<Node *> args) {
    return new Node(funcName, args);
  }
  /*
  Node *onNewPredicate(std::string &predName) {
      std::cout << __FUNCTION__ << std::endl;
    return new Node(predName, args, true);
  }*/
  Node *onNewFunction(Node *node, std::vector<Node *> args) {
    assert(node != nullptr);
    assert(node->children_.empty());
    for (auto &arg : args)
      assert(arg != nullptr);
    node->children_ = args;
    node->is_pred_ = true;
    return node;
  }
  Node *onNewPredicate(Node *node, std::vector<Node *> args) {
    assert(node != nullptr);
    assert(node->children_.empty());
    for (auto &arg : args)
      assert(arg != nullptr);
    node->children_ = args;
    node->is_pred_ = true;
    return node;
  }
  Node *onOperator(node_type_t type, vector<Node *> children) {
    for (auto &child : children)
      assert(child != nullptr);
    return new Node(type, children);
  }

private:
  Ast *parse_helper(std::istream &stream);

  FE::Parser *parser = nullptr;
  FE::Scanner *scanner = nullptr;

  Node *root_ = nullptr;
};

} // namespace FE
#endif /* END __DRIVER_HPP__ */
