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
#include "util.hpp"

class Dag;
class DagNode;

class DagBuilder {
public:
  /*
   * Builds the DG from the given AST
   * @param ast: AST of a formula
   * @return: Created Dag object
   */
  static Dag *BuildDag(const Ast *ast) {
    DagBuilder *builder = new DagBuilder();
    builder->GenerateSubFormulae(ast->root_);
    builder->GatherEqualityConstraints(ast->root_);
    builder->GatherInEqualityConstraints(ast->root_);
    std::cout << "SubFormulae:       " << builder->sub_formulae_ << std::endl;
    std::cout << "SubFormulae(Eq):   " << builder->sub_formulae_equality_
              << std::endl;
    std::cout << "SubFormulae(InEq): " << builder->sub_formulae_inequality_
              << std::endl;
    std::cout << "========== DagView ==========" << std::endl;
    for (auto &sf : builder->sub_formulae_) {
      std::cout << sf->ToFormula() << "\n" << sf->DagIDView() << std::endl;
    }
    std::cout << "=============================" << std::endl;
    Dag *dag = new Dag(builder->sub_formulae_, builder->sub_formulae_equality_,
                       builder->sub_formulae_inequality_);
    delete builder;
    return dag;
  }

private:
  /*
   * Generate sub formulae from the AST by post order traversal
   * Sub formulae are stored using DagNode structure.
   * @param root: AST root
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

  /*
   * Creates and adds sub formula to data structures
   * @param ast_node: AST node
   * @return: Return true, if a new sub formula is added.
   */
  bool AddSubFormulae(const Node *ast_node) {
    assert(ast_node->type_ == node_type_func ||
           ast_node->type_ == node_type_var);
    DagNode *dag_node = nullptr;
    bool created = ToDagNode(ast_node, dag_node);
    if (created) {
      sub_formulae_.push_back(dag_node);
      if (ast_node->type_ == node_type_var) {
        ast_node_dag_node_map_[ast_node->var_name_] = dag_node;
      }
    }
    ast_fun_dag_node_map_[ast_node] = dag_node;
    return created;
  }

  /*
   * Converts an AST Node to Dag Node
   * @param ast_node: The ast node, for which dag node will be created
   * @param dag_node: The dag node corresponding to the ast node is returned.
   * Returns true, if a new DAG node is created, and false if no node is created
   */
  bool ToDagNode(const Node *ast_node, DagNode *&dag_node) {

    dag_node = DiscoverDagNode(ast_node);

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
      // Child must be available as we have already covered the children
      assert(ast_fun_dag_node_map_.find(ast_node->children_[i]) !=
             ast_fun_dag_node_map_.end());
      auto child_dag_node = ast_fun_dag_node_map_[ast_node->children_[i]];
      dag_node->args_.push_back(child_dag_node->GetId());
      dag_node->n_args_++;
      child_dag_node->ccpar_.push_back(dag_node->GetId());
      dag_node->AddChild(child_dag_node);
    }
    return true;
  }

  /*
   * Discovers if a similar dag node corresponding to the ast node exists
   * @param ast_node: The ast node
   * @return: The DAG node for the ast node.
   *    If no similar DAG node exist, NULL is returned
   *    If similar(same function signature) function exist, then the function
   * DAG node is returned If signature miss matches i.e function names are same
   * but no of parameter is different then aborted
   */
  DagNode *DiscoverDagNode(const Node *ast_node) {
    assert(ast_node->type_ == node_type_func ||
           ast_node->type_ == node_type_var);

    if (ast_node->type_ == node_type_var) {
      auto it = ast_node_dag_node_map_.find(ast_node->var_name_);
      return it == ast_node_dag_node_map_.end() ? nullptr : it->second;
    }

    auto sf_it = std::find_if(
        sub_formulae_.begin(), sub_formulae_.end(),
        [&](const DagNode *dag_node) {
          if (dag_node->IsFunction() &&
              ast_node->func_name_ == dag_node->GetName()) {
            if (ast_node->children_.size() != dag_node->NumChildren()) {
              std::cerr << std::endl
                        << "Function signature miss match" << std::endl
                        << "F1: " << dag_node->ToFormula() << std::endl
                        << "F2: " << ast_node->ToFormula() << std::endl;
              abort();
            }

            // Match children
            bool children_matched = true;
            for (size_t i = 0; i < dag_node->NumChildren(); i++) {
              auto ast_child_dag_node =
                  ast_fun_dag_node_map_[ast_node->children_[i]];
              children_matched = children_matched &&
                                 (ast_child_dag_node == dag_node->children_[i]);
            }
            return children_matched;
          }
          return false;
        });
    return sf_it == sub_formulae_.end() ? nullptr : *sf_it;
  }

  /**************** Find Equality & Inequality constrains ****************/

  /*
   * Collects all the Equality Constraints
   * All the DAG nodes are generated, before the call.
   * @param root: AST root
   */
  void GatherEqualityConstraints(const Node *root) {
    if (!root)
      return;
    if (root->type_ == node_type_equal) {
      assert(root->children_.size() == 2);

      sub_formulae_equality_.push_back({DiscoverDagNode(root->children_[0]),
                                        DiscoverDagNode(root->children_[1])});
    }
    for (size_t i = 0; i < root->children_.size(); i++)
      GatherEqualityConstraints(root->children_[i]);
  }

  /*
   * Collects all the Inequality Constraints
   * All the DAG nodes are generated, before the call.
   * @param root: AST root
   */
  void GatherInEqualityConstraints(const Node *root) {
    if (!root)
      return;
    if (root->type_ == node_type_not_equal) {
      assert(root->children_.size() == 2);

      sub_formulae_inequality_.push_back({DiscoverDagNode(root->children_[0]),
                                          DiscoverDagNode(root->children_[1])});
    }
    for (size_t i = 0; i < root->children_.size(); i++)
      GatherInEqualityConstraints(root->children_[i]);
  }

  DagBuilder() {}

  std::vector<DagNode *> sub_formulae_;
  std::vector<std::pair<DagNode *, DagNode *>> sub_formulae_equality_;
  std::vector<std::pair<DagNode *, DagNode *>> sub_formulae_inequality_;
  std::unordered_map<const Node *, DagNode *> ast_fun_dag_node_map_;
  std::unordered_map<std::string, DagNode *> ast_node_dag_node_map_;
  Dag *dag_ = nullptr;
};
#endif
