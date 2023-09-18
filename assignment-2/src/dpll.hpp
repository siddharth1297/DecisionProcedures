#ifndef DPLL_HPP
#define DPLL_HPP

#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

enum class ClauseState { Satisfied, Conflicting, Unresolved };

class BoolFunction {
public:
  BoolFunction() = delete;
  BoolFunction(const string &file_name);
  BoolFunction(const BoolFunction &bool_func);
  void ParseDimacsFile(const string &file_name);
  inline bool IsAssigned(const int &i) const {
    assert(i != 0);
    return assignment_[abs(i)] != -1;
  }
  inline bool AssignedValue(const int &i) const {
    assert(i != 0 && IsAssigned(i));
    return i > 0 ? assignment_[i] : !assignment_[-1 * i];
  }
  ClauseState Apply(const int id, const int value);
  ClauseState UpdateState();
  void DumpAssignment();

  size_t n_clauses_ = 0;
  size_t n_var_ = 0;
  vector<vector<int>> functions_;
  vector<int> assignment_;
  vector<pair<int, int>> frequency_sign_;
};

class Dpll {
public:
  Dpll() = delete;
  Dpll(BoolFunction boolFunction);
  inline bool Solve() { return SolveDPLL(bool_func_); }

private:
  ClauseState ApplyBCP(BoolFunction &func);
  bool SolveDPLL(BoolFunction bool_func);
  const BoolFunction bool_func_;
  pair<int, int> ChooseUnassignedVar(const BoolFunction &func);
};
#endif
