#ifndef __DAG_HPP__
#define __DAG_HPP__

#include <iostream>
#include <unordered_set>

class DagBuilder;

enum class dag_node_type_t { dag_node_type_fun, dag_node_type_var };

class DagNode {
public:
  DagNode(const dag_node_type_t type, const std::string &name)
      : id_(GetId()), fn_(name), n_args(0), cc(id_), type_(type) {}

  inline const bool IsFunction() const {
    return type_ == dag_node_type_t::dag_node_type_fun;
  }
  inline const size_t NumChildren() const {
    assert(IsFunction());
    return children_.size();
  }
  bool operator==(const DagNode &node) const {
    if (type_ != node.type_ || fn_ != node.fn_)
      return false;
    if (children_.size() != node.children_.size()) {
      std::cerr << "Function signature miss match" << std::endl
                << "F1: " << ToFormula() << std::endl
                << "F2: " << node.ToFormula() << std::endl;
      abort();
    }
    for (size_t i = 0; i < children_.size(); i++) {
      if (!(*(children_[i]) == *(node.children_[i])))
        return false;
    }
    return true;
  }
  std::string ToFormula() const {
    std::stringstream ss;
    ToFormula(ss);
    return ss.str();
  }
  void ToFormula(std::stringstream &ss) const {
    ss << fn_;
    if (type_ == dag_node_type_t::dag_node_type_var) {
      return;
    }

    ss << "(";
    for (size_t i = 0; i < children_.size(); i++) {
      children_[i]->ToFormula(ss);
      if (i + 1 < children_.size()) {
        ss << ",";
      }
    }
    ss << ")";
  }

  bool AddChild(DagNode *new_child) {
    for (auto &child : children_) {
      if (child == new_child) {
        return false;
      }
    }
    children_.push_back(new_child);
    return true;
  }
  const std::string &GetName() const { return fn_; }

private:
  static inline int GetId() { return ++id_cnt_; }
  static int id_cnt_;
  int id_;
  const std::string fn_;
  int n_args = 0;
  std::unordered_set<int> args;
  int cc = -1;
  std::unordered_set<int> ccpar;

  const dag_node_type_t type_;
  std::vector<DagNode *> children_;
};

int DagNode::id_cnt_ = 0;
class Dag {
private:
  friend class DagBuilder;
  Dag() {}
  bool AddNode() { NOT_IMPLEMENTED; }
};
#endif
