/*
 * Contains code for building DAG from AST.
 * Also, container for meta data.
 * TODO: Add ast validation:
 * 1. No negation before any variable
 */
#ifndef __DAG_BUILDER_HPP__
#define __DAG_BUILDER_HPP__

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "dag.hpp"

class Dag;
class DagNode;

class DagBuilder {
public:
  static Dag *BuildDag(const Ast *ast) {
    Dag *dag = new Dag();
    DagBuilder *builder = new DagBuilder();
    builder->GenerateSubFormulae(ast->root_);
    std::cout << builder->SubFormulaeToStr() << std::endl;
    builder->GatherEqualityConstraints(ast->root_);
    builder->GatherInEqualityConstraints(ast->root_);
    std::cout << builder->EqFormulaeToStr() << std::endl;
    std::cout << builder->InEqFormulaeToStr() << std::endl;
    return dag;
  }
  std::string SubFormulaeToStr() {
    std::stringstream ss;
    SubFormulaeToStrWithSS(ss);
    return ss.str();
  }
  void SubFormulaeToStrWithSS(std::stringstream &ss) {
    ss << "[";
    for (size_t i = 0; i < sub_formulae_.size(); i++) {
      sub_formulae_[i]->ToFormula(ss);
      if (i + 1 < sub_formulae_.size())
        ss << ", ";
    }
    ss << "]";
  }
  std::string EqFormulaeToStr() {
    std::stringstream ss;
    EqFormulaeToStrWithSS(ss);
    return ss.str();
  }
  void EqFormulaeToStrWithSS(std::stringstream &ss) {
    ss << "[";
    for (size_t i = 0; i < sub_formulae_equality_.size(); i++) {
      sub_formulae_equality_[i]->ToFormula(ss);
      if (i + 1 < sub_formulae_equality_.size())
        ss << ", ";
    }
    ss << "]";
  }

  std::string InEqFormulaeToStr() {
    std::stringstream ss;
    InEqFormulaeToStrWithSS(ss);
    return ss.str();
  }
  void InEqFormulaeToStrWithSS(std::stringstream &ss) {
    ss << "[";
    for (size_t i = 0; i < sub_formulae_inequality_.size(); i++) {
      sub_formulae_inequality_[i]->ToFormula(ss);
      if (i + 1 < sub_formulae_inequality_.size())
        ss << ", ";
    }
    ss << "]";
  }

private:
  /*
   * Generate sub formulae from the AST by post order traversal
   * Sub formulae are stored using DagNode structure.
   */
  void GenerateSubFormulae(const Node *root) {
    if (!root)
      return;
    for (auto &node : root->children_) {
      GenerateSubFormulae(node);
    }
    if (root->type_ == node_type_var || root->type_ == node_type_func) {
      AddSubFormulae(root);
    }
  }

  bool AddSubFormulae(const Node *ast_node) {
    assert(ast_node->type_ == node_type_func ||
           ast_node->type_ == node_type_var);
    DagNode *dag_node = nullptr;
    bool created = ToDagNode(ast_node, dag_node);
    if (created) {
      AddToSubFormulae(dag_node);
      if (ast_node->type_ == node_type_func)
        ast_dag_node_map_[ast_node] = dag_node;
    }
    return created;
  }

  bool ToDagNode(const Node *ast_node, DagNode *&dag_node) {
    assert(ast_node->type_ == node_type_func ||
           ast_node->type_ == node_type_var);
    if (ast_node->type_ != node_type_func)
      dag_node = FindEqualSignSubFormula(ast_node);

    if (dag_node != nullptr) {
      return false;
    }
    // Create a new node
    const dag_node_type_t dag_node_type =
        (ast_node->type_ == node_type_func
             ? dag_node_type_t::dag_node_type_fun
             : dag_node_type_t::dag_node_type_var);
    dag_node = new DagNode(dag_node_type, ast_node->GetName());
    for (size_t i = 0; i < ast_node->children_.size(); i++) {
      // Child must be available as post order traversal
      auto child_dag_node = FindEqualSignSubFormula(ast_node->children_[i]);
      if (child_dag_node == nullptr) {
        std::cerr << "Node:: " << ast_node->children_[i] << std::endl;
      }
      assert(child_dag_node != nullptr);
      dag_node->AddChild(child_dag_node);
    }
    return true;
  }

  DagNode *FindEqualSignSubFormula(const Node *ast_node) {
    assert(ast_node->type_ == node_type_func ||
           ast_node->type_ == node_type_var);
    if (ast_node->type_ == node_type_func) {
      // function must be available
      auto it = ast_dag_node_map_.find(ast_node);
      return it->second;
    }
    // First search in the sub_formulae_;
    auto it = std::find_if(
        sub_formulae_.begin(), sub_formulae_.end(), [&](DagNode *dg_node) {
          // For variable, only match name
          if (!dg_node->IsFunction())
            return ast_node->var_name_ == dg_node->GetName();

          // TODO: Remove it as we are using only functions
          // For function, name & signature must match
          if ((ast_node->func_name_ != dg_node->GetName()) ||
              (ast_node->children_.size() != dg_node->NumChildren())) {
            return false;
          }

          return true;
        });
    return it == sub_formulae_.end() ? nullptr : *it;
  }

  /********************************************** Find Equality
   * ******************/
  void GatherEqualityConstraints(Node *root) {
    if (!root)
      return;
    if (root->type_ == node_type_equal) {
      assert(root->children_.size() == 2);
      AddToSubFormulaeEqual(FindEqualSignSubFormula(root->children_[0]));
      AddToSubFormulaeEqual(FindEqualSignSubFormula(root->children_[1]));
    }
    for (size_t i = 0; i < root->children_.size(); i++)
      GatherEqualityConstraints(root->children_[i]);
  }
  void GatherInEqualityConstraints(Node *root) {
    if (!root)
      return;
    if (root->type_ == node_type_not_equal) {
      assert(root->children_.size() == 2);
      AddToSubFormulaeInEqual(FindEqualSignSubFormula(root->children_[0]));
      AddToSubFormulaeInEqual(FindEqualSignSubFormula(root->children_[1]));
    }
    for (size_t i = 0; i < root->children_.size(); i++)
      GatherInEqualityConstraints(root->children_[i]);
  }

  bool AddToSubFormulae(DagNode *dag_node) {
    // Every element must be unique
    for (auto &sf : sub_formulae_)
      if (*dag_node == *sf)
        return false;
    sub_formulae_.push_back(dag_node);
    return true;
  }
  bool AddToSubFormulaeEqual(DagNode *dag_node) {
    // Every element must be unique
    for (auto &sf : sub_formulae_equality_)
      if (*dag_node == *sf)
        return false;
    sub_formulae_equality_.push_back(dag_node);
    return true;
  }
  bool AddToSubFormulaeInEqual(DagNode *dag_node) {
    // Every element must be unique
    for (auto &sf : sub_formulae_inequality_)
      if (*dag_node == *sf)
        return false;
    sub_formulae_inequality_.push_back(dag_node);
    return true;
  }
  DagBuilder() {}
  std::vector<DagNode *> sub_formulae_;
  std::vector<DagNode *> sub_formulae_equality_;
  std::vector<DagNode *> sub_formulae_inequality_;
  std::unordered_map<const Node *, DagNode *> ast_dag_node_map_;
  Ast *ast_ = nullptr;
  Dag *dag_ = nullptr;
};
#endif
