#include <chrono>
#include <ctime>
#include <iostream>
#include <unistd.h>

#include "dpll.hpp"

using namespace std;

int main(int argv, char **argc) {
  if (argv < 2) {
    cout << "Usgae: ./main {file.cnf}" << endl;
    return EXIT_FAILURE;
  }

  const string file_name = argc[1];
  auto bool_func = BoolFunction(file_name);
  auto dpll = Dpll(bool_func);
  auto start = std::chrono::system_clock::now();
  std::time_t start_time = std::chrono::system_clock::to_time_t(start);
  cout << "Solver Starting\t" << std::ctime(&start_time);
  if (dpll.Solve()) {
    cout << "SATISFIED" << endl;
  } else {
    cout << "UNSATISFIED" << endl;
  }
  auto end = std::chrono::system_clock::now();
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);
  cout << "Solver Ended\t" << std::ctime(&end_time);
  return 0;
}
