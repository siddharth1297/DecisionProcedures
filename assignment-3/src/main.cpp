#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>

#include "fe_driver.hpp"

#include "dag.hpp"
#include "dag_builder.hpp"

using namespace std;

int main(const int argc, const char **argv) {
  if (argc != 2 || std::strncmp(argv[1], "-h", 2) == 0) {
    std::cerr << "./main {filename}" << std::endl;
    std::cerr << "cat {filename} | ./main -o" << std::endl;
    std::cerr << "./main -h" << std::endl;
    return (EXIT_FAILURE);
  }

  Ast *ast = nullptr;

  FE::Driver driver;
  // Read from piped input
  if (std::strncmp(argv[1], "-o", 2) == 0) {
    ast = driver.parse(std::cin);
  } else {
    // Read from file, given as 1st argument
    ast = driver.parse(argv[1]);
  }

  assert(ast->root_);
  cout << "Parse Tree" << std::endl << *(ast) << endl;
  cout << "Parsed Tree To Formula" << std::endl << ast->ToFormula() << endl;
  cout << "---------------Building DAG---------------" << endl;
  Dag *dag = DagBuilder::BuildDag(ast);
  // cout << DagBuilder::SubFormulaeToStr() << std::endl;
  // cout << dag << endl;
  delete dag;
  delete ast;
  return (EXIT_SUCCESS);
}
