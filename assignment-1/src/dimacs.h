#ifndef DIMACS_H
#define DIMACS_H

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *outfd;

typedef struct variable_container_t {
  char var_name[MAX_NAME_SIZE];
} variable_container_t;

extern variable_container_t *var_list;
static int variables = 0;
static int highestVarIdx = 1;
static int clauses = 0;

static int findAddVarIfNotPresent(char *var_name,
                                  variable_container_t *var_list) {
  assert(strlen(var_name) > 0);
  int idx = 1;
  for (idx = 1; idx < highestVarIdx; idx++) {
    if (strcmp(var_list[idx].var_name, var_name) == 0)
      break;
  }
  if (idx == (highestVarIdx)) {
    strcpy(var_list[idx].var_name, var_name);
    // fprintf(outfd, "VARIABLE: %d:  \"%s\"\n", idx, var_list[idx].var_name);
    variables++;
    highestVarIdx++;
    assert(variables == highestVarIdx - 1);
  }
  return idx;
}
static int findVar(char *var_name) {
  assert(strlen(var_name) > 0);
  int idx = 1;
  for (idx = 1; idx < highestVarIdx; idx++) {
    if (strcmp(var_list[idx].var_name, var_name) == 0)
      break;
  }
  // assert();
  return idx;
}

static void addVariablesFromTree(node_t *node, variable_container_t *var_list) {
  if (!node)
    return;
  if (node->type_ == node_type_var) {
    findAddVarIfNotPresent(node->var_name_, var_list);
  }
  addVariablesFromTree(node->left_, var_list);
  addVariablesFromTree(node->right_, var_list);
}

static int initVarList(node_t *tsetinNode) {
  // nVar is the maximum no of variables
  int nVar = countOpTypeNode(tsetinNode, node_type_var); // Rturns #literals
  var_list = (variable_container_t *)calloc(sizeof(variable_container_t), nVar);
  addVariablesFromTree(tsetinNode, var_list);
  fprintf(outfd, "total literals: %d Variables: %d\n", nVar, variables);
  for (int i = 0; i < highestVarIdx; i++) {
    fprintf(outfd, "{%d: %s} ", i, var_list[i].var_name);
  }
  fprintf(outfd, "\n");
  return nVar;
}

static void dumpCNF(node_t *root, FILE *file, int *lineEnded) {
  assert(root);
  if ((isNodeTypeOp(root->type_) && root->type_ == node_type_not)) {
    // fprintf(file, "-%s ", (root->left_->var_name_));
    char location[MAX_NAME_SIZE];
    sprintf(location, "%d", findVar(root->left_->var_name_));
    fprintf(file, "-%s ", location);
    assert(root->left_->type_ == node_type_var);
    assert(root->left_->left_ == NULL);
    assert(root->right_ == NULL);
    *lineEnded = 0;
    return;
  }
  if (root->type_ == node_type_var) {
    // fprintf(file, "%s ", (root->var_name_));
    char location[MAX_NAME_SIZE];
    sprintf(location, "%d", findVar(root->var_name_));
    fprintf(file, "%s ", location);
    *lineEnded = 0;
    return;
  }

  dumpCNF(root->left_, file, lineEnded);
  if (!(*lineEnded) && root->type_ == node_type_and) {
    fprintf(file, "0\n");
    *lineEnded = 1;
  }
  dumpCNF(root->right_, file, lineEnded);
  if (!(*lineEnded) && root->type_ == node_type_and) {
    fprintf(file, "0\n");
    *lineEnded = 1;
  }
}

static int toDimacs(node_t *tsetinNode, char *fileName) {
  FILE *file = fopen(fileName, "w");
  initVarList(tsetinNode);
  clauses = countOpTypeNode(tsetinNode, node_type_and) + 1;
  fprintf(file, "p cnf %d %d\n", clauses, variables);
  int lineEnded = 1;
  dumpCNF(tsetinNode, file, &lineEnded);
  //assert(lineEnded == 1);
  if(lineEnded != 1) {fprintf(file, "0\n");}
  // The above assertion always holds, but in case of expression like: {p}, it fails.
  // So use the if condition
  fclose(file);
  return 1;
}

#endif
