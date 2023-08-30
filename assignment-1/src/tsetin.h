#ifndef TSETIN_H
#define TSETIN_H

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

static int funId = 0;
static int varId = 0;
static node_t **functionArr = NULL;
static int functionArrIdx = 0;

void solve(node_t *root);
void removeBiImplication(node_t *root);
void removeImplication(node_t *root);
void removeDoubleNeg(node_t *root);
void applyDemorgan(node_t *root);
void applyDistibutive(node_t *root);
void applyDistibutiveOrOverAnd(node_t *root);

node_t *constructTsetinTree();

/*
 * Initializes data structure
 */
int tsetinHelperInit(node_t *root) {
  int nNodes = countASTNode(root);
  functionArr = (node_t **)calloc(sizeof(node_t *), nNodes);
  assert(functionArr != NULL);
  functionArrIdx = 0;
  funId = varId = 0;
  return nNodes;
}

/*
 * Free structures
 */
void tsetinHelperDestroy(node_t *root) {
  free(functionArr);
  functionArrIdx = 0;
}

/*
 * Set variable name and funcion name to operator nodes.
 * Assumption: No variable in the given formula should start with a1 and onward
 */
void setNamesToOpNode(node_t *root) {
  if (!root)
    return;
  if (isNodeTypeOp(root->type_)) {
    sprintf(root->func_name_, "%s%d", "F", ++funId);
    sprintf(root->var_name_, "%s%d", "a", ++varId);
  }
  setNamesToOpNode(root->left_);
  setNamesToOpNode(root->right_);
}

/*
 * Collects the operator nodes  by traversing the AST and keeps in an array.
 */
void createTsetinNodes(node_t *root) {
  if (!root)
    return;
  if (isNodeTypeOp(root->type_)) {
    node_t *new_node = alloc_node();
    new_node->type_ = node_type_bi_implication;
    strcpy(new_node->func_name_, root->func_name_);

    // left node is the variable itself
    new_node->left_ = alloc_node();
    new_node->left_->type_ = node_type_var;
    strcpy(new_node->left_->var_name_, root->var_name_);

    // right child is the subformula involving root
    new_node->right_ = alloc_node();
    new_node->right_->type_ = root->type_;
    // strcpy(new_node->right_->func_name_, root->func_name_);

    new_node->right_->left_ = alloc_node();
    new_node->right_->left_->type_ = node_type_var;
    assert(strlen(root->left_->var_name_) != 0);
    strcpy(new_node->right_->left_->var_name_, root->left_->var_name_);

    if (root->right_) {
      new_node->right_->right_ = alloc_node();
      new_node->right_->right_->type_ = node_type_var;
      assert(strlen(root->right_->var_name_) != 0);
      strcpy(new_node->right_->right_->var_name_, root->right_->var_name_);
    } else {
      assert(root->type_ == node_type_not);
    }

    functionArr[functionArrIdx++] = new_node;
  }
  createTsetinNodes(root->left_);
  createTsetinNodes(root->right_);
}

/*
 * Converts an AST to Tsetin tree
 * Call this function from outside.
 */
node_t *toTestin(node_t *root) {
  int nNodes = tsetinHelperInit(root);
  setNamesToOpNode(root);
  printAST(root);
  createTsetinNodes(root);
  node_t *tsetinRoot = NULL;

  for (int i = 0; i < functionArrIdx; i++) {
    fprintf(outfd, "index: %d\n", i);
    solve(functionArr[i]);
  }

  for (int i = 0; i < functionArrIdx; i++) {
    fprintf(outfd, "index: %d\n", i);
    printAST(functionArr[i]);
    dumpAST(functionArr[i]);
    dumpASTMinimal(functionArr[i]);
  }

  tsetinRoot = constructTsetinTree();
  return tsetinRoot;
}

/*
 * Constructs the Tsetin tree by traversing each subformula from the array along with the start variable.
 */
node_t *constructTsetinTree() {
  char newVar[MAX_NAME_SIZE] = "a1";
  printf("%s\n", newVar);
  node_t *head = alloc_node();
  head->type_ = node_type_var;
  strcpy(head->var_name_, newVar);
  for (int i = 0; i < functionArrIdx; i++) {
    node_t *new_node = alloc_node();
    new_node->type_ = node_type_and;
    new_node->left_ = head;
    head = new_node;
    head->right_ = functionArr[i];
  }
  return head;
}

/******************************** Solve ********************************/

void solve(node_t *root) {
  fprintf(outfd, "==========solving========\n");
  assert(root != NULL);
  assert(root->type_ == node_type_bi_implication);

  printAST(root);
  dumpAST(root);
  removeBiImplication(root);
  assert(countOpTypeNode(root, node_type_bi_implication) == 0);
  fprintf(outfd, "Removed all Biimplication\n");
  printAST(root);

  removeImplication(root);
  assert(countOpTypeNode(root, node_type_implication) == 0);
  fprintf(outfd, "Removed all Implication\n");
  printAST(root);

  int avail = 0;
  do {
    avail = 0;
    while (countDoubleNeg(root)) {
      avail = 1;
      removeDoubleNeg(root);
    }
    // printAST(root); dumpAST(root);
    while (countDemorgan(root)) {
      avail = 1;
      applyDemorgan(root);
    }
    // printAST(root); dumpAST(root);
    while (countDistributiveOrOverAnd(root)) {
      avail = 1;
      applyDistibutiveOrOverAnd(root);
    }
    // printAST(root); dumpAST(root);
  } while (avail);

  fprintf(outfd, "============DONE====================\n");
}

/*
 * Removes <-> from an tree
 */
void removeBiImplication(node_t *root) {
  if (!root)
    return;
  if (root->type_ == node_type_bi_implication) {
    // convert the root to and
    root->type_ = node_type_and;
    node_t *left = root->left_, *right = root->right_;
    root->left_ = alloc_node();
    root->left_->type_ = node_type_implication;
    root->left_->left_ = left;
    root->left_->right_ = right;

    root->right_ = alloc_node();
    root->right_->type_ = node_type_implication;
    root->right_->left_ = deep_clone_node(right);
    root->right_->right_ = deep_clone_node(left);
  }
  assert(root->type_ != node_type_bi_implication);
  removeBiImplication(root->left_);
  removeBiImplication(root->right_);
}

/*
 * Removes -> from an tree
 */
void removeImplication(node_t *root) {
  if (!root)
    return;
  if (root->type_ == node_type_implication) {
    // convert the current node operator to or
    root->type_ = node_type_or;

    node_t *left = root->left_;
    // allocate a not node
    root->left_ = alloc_node();
    root->left_->type_ = node_type_not;
    root->left_->left_ = left;
  }
  assert(root->type_ != node_type_implication);
  removeImplication(root->left_);
  removeImplication(root->right_);
}

/*
 * Removes !! from the tree
 */
void removeDoubleNeg(node_t *root) {
  if (!root)
    return;
  if (root->left_ && root->left_->type_ == node_type_not &&
      root->left_->left_ && root->left_->left_->type_ == node_type_not) {
    assert(root->left_->right_ == NULL);
    assert(root->left_->left_->right_ == NULL);
    root->left_ = root->left_->left_->left_;
  }
  removeDoubleNeg(root->left_);
  removeDoubleNeg(root->right_);
}

/*
 * Applies Demorgan's law if possible
 * If any ! or & is next to !
 * Removes double negation after applying the rule
 */
void applyDemorgan(node_t *root) {
  if (!root)
    return;
  if (root->type_ == node_type_not && root->left_ &&
      root->left_->type_ != node_type_var) {
    // fprintf(outfd, "Apply demorgan on: %s\n", opToStr[root->type_]);
    // printAST(root);
    assert(root->right_ == NULL);
    assert(root->left_->type_ == node_type_or ||
           root->left_->type_ == node_type_and);
    assert(
        root->left_->type_ !=
        node_type_not); // Bcs remove double negtion before processing the node
    // change the nodes's type from not to and or or
    root->type_ =
        (root->left_->type_ == node_type_or) ? node_type_and : node_type_or;

    node_t *left = root->left_->left_, *right = root->left_->right_;
    // Set the above pointers to NULL
    root->left_->left_ = root->left_->right_ = NULL;

    // Allocate a new node for the right child
    root->right_ = alloc_node();
    root->left_->type_ = root->right_->type_ = node_type_not;

    // change the orginial pointers to the left of the child pointers
    root->left_->left_ = left;
    root->right_->left_ = right;
    removeDoubleNeg(root);
  }
  applyDemorgan(root->left_);
  applyDemorgan(root->right_);
}

void applyDistributive(node_t *root) { applyDistibutiveOrOverAnd(root); }

/*
 * Applies Distributive rule of the following type. 
 * x | (y & z) or (x&y) | (c&d) or (y & z) | x
 */
void applyDistibutiveOrOverAnd(node_t *root) {
  if (!root)
    return;

  if (root->type_ == node_type_or) {
    assert(root->left_ && root->right_);
  }
  // (x & y) | (c & d)
  if (root->type_ == node_type_or && isNodeTypeOp(root->left_->type_) &&
      root->left_->type_ == node_type_and &&
      isNodeTypeOp(root->right_->type_) &&
      root->right_->type_ == node_type_and) {
    // change op type
    root->type_ = node_type_and;

    node_t *left = root->left_, *right = root->right_;

    root->left_ = alloc_node();
    root->right_ = alloc_node();
    root->left_->type_ = root->right_->type_ = node_type_or;

    root->left_->left_ = left->left_;
    root->left_->right_ = deep_clone_node(right);

    root->right_->left_ = left->right_;
    root->right_->right_ = right->right_;
  }
  // patter: x | (y & z)
  if (root->type_ == node_type_or && isNodeTypeOp(root->right_->type_) &&
      root->right_->type_ == node_type_and) {
    assert(root->left_->type_ == node_type_not ||
           root->left_->type_ == node_type_var);
    root->type_ = node_type_and;

    node_t *new_node = alloc_node();
    new_node->left_ = root->left_;
    root->left_ = new_node;

    root->left_->type_ = root->right_->type_ = node_type_or;

    root->left_->right_ = deep_clone_node(root->right_->left_);
    root->right_->left_ = deep_clone_node(root->left_->left_);
  }
  // pattern: (y & z) | x
  if (root->type_ == node_type_or && isNodeTypeOp(root->left_->type_) &&
      root->left_->type_ == node_type_and) {
    assert(root->right_->type_ == node_type_not ||
           root->right_->type_ == node_type_var);
    printAST(root);
    root->type_ = node_type_and;

    node_t *new_node = alloc_node();
    new_node->right_ = root->right_;
    root->right_ = new_node;

    root->left_->type_ = root->right_->type_ = node_type_or;

    root->right_->left_ = deep_clone_node(root->left_->right_);
    root->left_->right_ = deep_clone_node(root->right_->right_);

    // printAST(root);
  }

  applyDistibutiveOrOverAnd(root->left_);
  applyDistibutiveOrOverAnd(root->right_);
}
/***********************************************************************/
#endif
