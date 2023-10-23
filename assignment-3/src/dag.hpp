#ifndef __DAG_HPP__
#define __DAG_HPP__

#include <iostream>
#include <unordered_set>

class DagBuilder;

enum class dag_node_type_t { dag_node_type_fun, dag_node_type_var };

class DagNode {
public:
  DagNode() = delete;

  DagNode(const dag_node_type_t type, const std::string &name)
      : id_(GetNewId()), fn_(name), n_args(0), cc(id_), type_(type) {}

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
    /*
    if (type_ != node.type_ || fn_ != node.fn_ ||
        children_.size() != node.children_.size())
      return false;
    for (size_t i = 0; i < children_.size(); i++) {
      if (!(*(children_[i]) == *(node.children_[i])))
        return false;
    }
    return true;
    */
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

  friend class DagBuilder;
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
  int n_args = 0;
  std::unordered_set<int> args;
  int cc = -1;
  std::unordered_set<int> ccpar;

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
private:
  friend class DagBuilder;
  Dag() {}
  bool AddNode() { NOT_IMPLEMENTED; }
};
#endif
