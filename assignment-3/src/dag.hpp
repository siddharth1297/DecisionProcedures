#ifndef __DAG_HPP__
#define __DAG_HPP__

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "util.hpp"

class DagBuilder;

enum class dag_node_type_t { dag_node_type_fun, dag_node_type_var };

class DagNode {
public:
  DagNode() = delete;

  DagNode(const dag_node_type_t type, const std::string &name)
      : id_(GetNewId()), fn_(name), n_args_(0), cc_(id_), type_(type) {}

  inline const bool IsFunction() const {
    return type_ == dag_node_type_t::dag_node_type_fun;
  }

  inline const size_t NumChildren() const {
    assert(IsFunction());
    return children_.size();
  }

  inline bool operator==(const DagNode *node) const {
    return this->GetHash() == node->GetHash();
  }

  inline bool operator==(const DagNode &node) const {
    return GetHash() == node.GetHash();
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

  inline void AddChild(DagNode *new_child) { children_.push_back(new_child); }

  inline const std::string &GetName() const { return fn_; }

  inline const int GetId() const { return id_; }

  std::string GetFullInfo() {
    std::stringstream ss;
    ss << "{" << id_ << ", " << fn_ << ", " << n_args_ << ", " << args_ << ", "
       << cc_ << ", " << ccpar_ << "}";
    return ss.str();
  }

  friend class DagBuilder;
  friend class Dag;
  friend hash<DagNode>;
  friend hash<DagNode *>;

  friend ostream &operator<<(ostream &os, const DagNode *node) {
    os << *node;
    return os;
  }

  friend ostream &operator<<(ostream &os, const DagNode &node) {
    os << node.fn_;
    if (node.type_ == dag_node_type_t::dag_node_type_var)
      return os;
    os << "(";
    for (size_t i = 0; i < node.children_.size(); i++) {
      os << *(node.children_[i]);
      if (i + 1 < node.children_.size()) {
        os << ",";
      }
    }
    os << ")";
    return os;
  }

  std::string DagIDView() {
    std::stringstream ss;
    DagIDView(ss);
    return ss.str();
  }

  void DagIDView(std::stringstream &ss) {
    ss << "--------------------------------------" << endl;
    queue<const DagNode *> q;
    ss << "["
       << "(" << id_ << ")"
       << "]" << std::endl;
    q.push(this);
    while (!q.empty()) {
      ss << "[";
      size_t len = q.size();
      while (len--) {
        const DagNode *node = q.front();
        q.pop();
        ss << "(";
        for (size_t i = 0; i < node->children_.size(); i++) {
          auto &nd = node->children_[i];
          assert(nd != nullptr);
          q.push(nd);
          ss << nd->id_;
          if (i + 1 != node->children_.size())
            ss << " ";
        }
        ss << ")";
        if (len > 0)
          ss << " ";
      }
      ss << "]" << endl;
    }
    ss << "--------------------------------------" << endl;
  }

private:
  inline size_t GetHash() const {
    return hash<int>()(id_) ^ hash<string>()(fn_);
  }

  static inline int GetNewId() { return ++id_cnt_; }
  static int id_cnt_;
  const int id_;
  const std::string fn_;
  int n_args_ = 0;
  std::vector<int> args_;
  int cc_;
  std::vector<int> ccpar_;

  const dag_node_type_t type_;
  std::vector<DagNode *> children_;
};

int DagNode::id_cnt_ = 0;

namespace std {
template <> class hash<DagNode> {
public:
  size_t operator()(const DagNode &dag_node) const {
    return dag_node.GetHash();
  }
};
} // namespace std

namespace std {
template <> class hash<DagNode *> {
public:
  size_t operator()(const DagNode *dag_node) const {
    return dag_node->GetHash();
  }
};
} // namespace std

class Dag {
public:
  ~Dag() {
    for (auto &frm : sub_formulae_) {
      delete frm;
    }
  }
  bool Decision() {
    bool res = true;
    for (auto &[frm1, frm2] : sub_formulae_equality_) {
      Merge(frm1->id_, frm2->id_);
    }
    for (auto &[frm1, frm2] : sub_formulae_inequality_) {
      if (Find(frm1->id_) == Find(frm2->id_)) {
        std::cout << "InEq in same congruence class: "
                  << "[" << frm1 << ", " << frm2 << "]" << std::endl;
        res = false;
      }
    }

    return res;
  }

private:
  friend class DagBuilder;
  Dag(const std::vector<DagNode *> sub_formulae,
      const std::vector<std::pair<DagNode *, DagNode *>> sub_formulae_equality,
      const std::vector<std::pair<DagNode *, DagNode *>>
          sub_formulae_inequality)
      : sub_formulae_(sub_formulae),
        sub_formulae_equality_(sub_formulae_equality),
        sub_formulae_inequality_(sub_formulae_inequality) {
    for (auto &node : sub_formulae) {
      id_node_map_[node->GetId()] = node;
    }
    std::cout << "Node Details at DAG initialization" << std::endl;
    for (auto it : id_node_map_) {
      std::cout << it.second << "\t" << it.second->GetFullInfo() << std::endl;
    }
  }

  /*
   * Returns a DAG node with identifier id
   * @param id: identifier of the DAG Node
   * @return: DAG node having id_ equals to id
   */
  inline DagNode *Node(const int id) { return id_node_map_[id]; }

  /*
   * Returns the congruence class
   * @param id: Identifier of a DAG node
   * @return: Returns the congruence class
   */
  int Find(const int id) {
    DagNode *node = Node(id);
    return (node->cc_ == id) ? node->cc_ : Find(node->cc_);
  }

  void Union(const int i, const int j) {
    DagNode *node1 = Node(i);
    DagNode *node2 = Node(j);

    node1->cc_ = node2->cc_;
    node2->ccpar_ = VectorSetUnion(node1->ccpar_, node2->ccpar_);
    node1->ccpar_.clear();
  }

  std::vector<int> &CCPar(const int id) { return Node(Find(id))->ccpar_; }

  bool Congruent(const int i, const int j) {
    DagNode *nodei = Node(i);
    DagNode *nodej = Node(j);

    if (nodei->fn_ != nodej->fn_ || nodei->n_args_ != nodej->n_args_) {
      return 0;
    }
    int n_args = nodei->n_args_;
    for (int i = 0; i < n_args; i++) {
      if (Find(nodei->args_[i]) != Find(nodej->args_[i])) {
        return 0;
      }
    }
    return true;
  }

  void Merge(const int i, const int j) {
    int root_i = Find(i);
    int root_j = Find(j);

    if (root_i != root_j) {
      auto par_i = CCPar(root_i);
      auto par_j = CCPar(root_j);

      Union(root_i, root_j);
      for (auto &t1 : par_i) {
        for (auto &t2 : par_j) {
          if (Find(t1) != Find(t2) && Congruent(t1, t2)) {
            Merge(t1, t2);
          }
        }
      }
    }
  }

  std::unordered_map<int, DagNode *> id_node_map_;
  const std::vector<DagNode *> sub_formulae_;
  const std::vector<std::pair<DagNode *, DagNode *>> sub_formulae_equality_;
  const std::vector<std::pair<DagNode *, DagNode *>> sub_formulae_inequality_;
};
#endif
