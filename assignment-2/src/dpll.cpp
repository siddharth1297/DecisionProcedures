#include "dpll.hpp"

Dpll::Dpll(BoolFunction bool_func) : bool_func_(bool_func) {}

/*
 * Applies value to the id variable
 */
ClauseState BoolFunction::Apply(const int id, const int value) {
  assert(id > 0);
  assert(value == 0 || value == 1);
  assignment_[id] = value;
  return UpdateState();
}

/*
 * Evaluates the function and sets meta structures.
 * If a variable in a clause is true, then the clause is deleted from the
 * function. If a variable in a clause is false, then the variable is deleted
 * from the clause.
 */
ClauseState BoolFunction::UpdateState() {
  for (int i = 0; i < functions_.size(); i++) {
    auto &clause = functions_[i];
    for (int j = 0; j < clause.size(); j++) {
      if (!IsAssigned(clause[j])) // Value not assigned yet
        continue;
      if (AssignedValue(clause[j])) {
        // clause is true, so remove the clause from the function
        functions_.erase(functions_.begin() + i);
        i--;
        break;
      }
      // Assigned value for this variable is false, so erase the variable from
      // the clause
      clause.erase(clause.begin() + j);
      j--;
    }
  }
  ClauseState function_state = ClauseState::Satisfied;
  pair<int, int> p = {0, 0};
  std::fill(frequency_sign_.begin(), frequency_sign_.end(), p);

  for (int i = 0; i < functions_.size(); i++) {
    auto &clause = functions_[i];
    if (clause.empty()) {
      function_state = ClauseState::Conflicting;
      // Don't return from here.
    }
    if (function_state != ClauseState::Conflicting) {
      function_state = ClauseState::Unresolved;
    }
    // count frequencies
    for (int j = 0; j < clause.size(); j++) {
      assert(!IsAssigned(clause[j]));
      if (clause[j] > 0)
        frequency_sign_[clause[j]].first++;
      else
        frequency_sign_[-1 * clause[j]].second++;
    }
  }
  return function_state;
}

/*
 * Applies unit resolution, if available
 */
ClauseState Dpll::ApplyBCP(BoolFunction &func) {
  if (func.functions_.empty())
    return ClauseState::Satisfied;
  bool has_bcp = false;
  do {
    has_bcp = false;
    for (auto &clause : func.functions_) {
      if (clause.size() != 1)
        continue;
      has_bcp = true;
      int id = abs(clause[0]), val = clause[0] > 0 ? 1 : 0;
      auto fun_state = func.Apply(id, val);
      if (fun_state != ClauseState::Unresolved)
        return fun_state;
      break; // Otherwise, may cause iterator invalidation exception
    }
  } while (has_bcp);
  return ClauseState::Unresolved;
}

/*
 * Choose a value and value to assign, on basis of some heuristic
 * Currently there is only one euristic.
 * Choose the variable having highest frequency.
 */
pair<int, int> Dpll::ChooseUnassignedVar(const BoolFunction &func) {
  assert(!func.functions_.empty());
  int idx = -1, val = -1, max_freq_cnt = -1;
  // Choose the value with highest frequency
  // value is the maximum among the signed count.
  for (int i = 1; i < func.frequency_sign_.size(); i++) {
    if (func.IsAssigned(i)) {
      assert(func.frequency_sign_[i].first == 0 &&
             func.frequency_sign_[i].second == 0);
      continue;
    }
    int freq_cnt =
        func.frequency_sign_[i].first + func.frequency_sign_[i].second;
    if (max_freq_cnt < freq_cnt) {
      max_freq_cnt = freq_cnt;
      idx = i;
      val = (func.frequency_sign_[i].first >= func.frequency_sign_[i].second);
    }
  }
  return {idx, val};
}

bool Dpll::SolveDPLL(const BoolFunction func) {
  // cout << "-----------------SOLVE-----------------------" << endl;
  // Apply BCP
  auto running_func = func;
  auto fun_state = ApplyBCP(running_func);
  if (fun_state != ClauseState::Unresolved) {
    if (fun_state == ClauseState::Satisfied) {
      running_func.DumpAssignment();
    }
    return fun_state == ClauseState::Satisfied;
  }

  // Choose a variable and value
  auto [var_idx, var_val] = ChooseUnassignedVar(running_func);
  if (var_idx == -1) {
    return false;
  }
  auto running_func_true = running_func;
  fun_state = running_func_true.Apply(var_idx, var_val);
  if (fun_state == ClauseState::Satisfied) {
    running_func_true.DumpAssignment();
    return true;
  }
  if (SolveDPLL(running_func_true))
    return true;

  // Apply the opposite value
  auto running_func_false = running_func;
  fun_state = running_func_false.Apply(var_idx, !var_val);
  if (fun_state == ClauseState::Satisfied) {
    running_func_false.DumpAssignment();
    return true;
  }
  if (SolveDPLL(running_func_false))
    return true;
  return false;
}
