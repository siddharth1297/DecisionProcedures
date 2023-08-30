#include <stdio.h>
#include <unistd.h>

#include "ast.h"
#include "dimacs.h"
#include "tsetin.h"

extern int yyparse();

node_t *root = NULL;
FILE *outfd;
variable_container_t *var_list = NULL;

int main() {
  outfd = stdout;
  // outfd = fopen("logfile", "w");
  yyparse();
  fprintf(outfd, "root:: %p\n", root);
  assert(root != NULL);
  printASTStats(root);
  printAST(root);
  node_t *tsetinRoot = toTestin(root);
  fprintf(outfd, "======================FINAL TREE=======================\n");
  printAST(tsetinRoot);
  fprintf(outfd, "=======================================================\n");
  fprintf(outfd, "===================FINAL EXPRESSION=====================\n");
  dumpASTMinimal(tsetinRoot);
  fprintf(outfd, "========================================================\n");
  toDimacs(tsetinRoot, "dimacs.cnf");
  fprintf(outfd, "Wrote to dimacs.cnf file\n");
  return 0;
}
