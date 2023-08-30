#ifndef AST_H
#define AST_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_SIZE 5

extern FILE *outfd;

typedef enum node_type {
  node_type_not,
  node_type_and,
  node_type_or,
  node_type_implication,
  node_type_bi_implication,
  node_type_var
} node_type_t;

static inline int isNodeTypeOp(node_type_t node_type) {
  return node_type >= node_type_not && node_type <= node_type_bi_implication;
}
static char *opToStr[] = {"!", "&", "|", "->", "<->", "VAR"};

static int id = 1;

typedef struct node_t {
  node_type_t type_;              // Type of the node
  char func_name_[MAX_NAME_SIZE]; // name of the (sub)function, set only if it
                                  // is an operator node
  char var_name_[MAX_NAME_SIZE];  // name of the variable
  int id_;                        // id of the variable
  int evaluated_;                 // If the value is set
  int value_;                     // Value of the node {0, 1}
  struct node_t *left_;           // left child
  struct node_t *right_;          // right child

} node_t;

static node_t *alloc_node() {
  node_t *node = (node_t *)malloc(sizeof(node_t));
  assert(node != NULL);
  bzero(node, sizeof(node_t));
  return node;
}

static node_t *create_node(node_type_t node_type, char *var_name, int evaluated,
                           int value, node_t *left, node_t *right) {
  node_t *node = alloc_node();
  node->type_ = node_type;
  strcpy(node->func_name_, "");
  strcpy(node->var_name_, var_name);
  if (node->type_ == node_type_var)
    node->id_ = id++; // TODO: Fix it
  node->evaluated_ = evaluated;
  node->value_ = node->evaluated_ ? value : -1;
  node->left_ = left;
  node->right_ = right;
  return node;
}

static node_t *shallow_clone_node(node_t *node) {
  node_t *new_node = alloc_node();
  memcpy(new_node, node, sizeof(node_t));
  return new_node;
}

static node_t *deep_clone_node(node_t *node) {
  if (node == NULL)
    return NULL;
  node_t *new_node = shallow_clone_node(node);
  new_node->left_ = deep_clone_node(new_node->left_);
  new_node->right_ = deep_clone_node(new_node->right_);
  return new_node;
}

static void printNode(node_t *node) {
  fprintf(outfd, "%s %s %d %d %d\n", opToStr[node->type_], node->var_name_,
          node->id_, node->value_, node->evaluated_);
}

static void printASTNode(node_t *node) {
  fprintf(outfd, " {");
  if (node == NULL) {
  } else if (isNodeTypeOp(node->type_)) {
    if (strlen(node->func_name_) != 0)
      fprintf(outfd, "\"%s\"", node->func_name_);
    if (strlen(node->var_name_) != 0)
      fprintf(outfd, "\"%s\"", node->var_name_);
    fprintf(outfd, "%s", opToStr[node->type_]);
  } else {
    assert(strlen(node->func_name_) == 0);
    fprintf(outfd, "%s", node->var_name_);
  }
  fprintf(outfd, "}");
}

static int countASTNode(const node_t *root) {
  if (!root)
    return 0;
  return 1 + countASTNode(root->left_) + countASTNode(root->right_);
}

static int countOpNode(const node_t *root) {
  if (!root)
    return 0;
  return isNodeTypeOp(root->type_) + countOpNode(root->left_) +
         countOpNode(root->right_);
}
static int countOpTypeNode(const node_t *root, const node_type_t node_type) {
  if (!root)
    return 0;
  return (root->type_ == node_type) + countOpTypeNode(root->left_, node_type) +
         countOpTypeNode(root->right_, node_type);
}

static int countDoubleNeg(const node_t *root) {
  if (!root)
    return 0;
  if ((isNodeTypeOp(root->type_) && root->type_ == node_type_not &&
       root->left_ && isNodeTypeOp(root->left_->type_) &&
       root->left_->type_ == node_type_not)) {
    return 1 + countDoubleNeg(root->left_->left_);
  }
  return countDoubleNeg(root->left_) + countDoubleNeg(root->right_);
}

static int countDemorgan(const node_t *root) {
  if (!root)
    return 0;
  return (isNodeTypeOp(root->type_) && (root->type_ == node_type_not) &&
          isNodeTypeOp(root->left_->type_) &&
          (root->left_->type_ == node_type_or ||
           root->left_->type_ == node_type_and)) +
         countDemorgan(root->left_) + countDemorgan(root->right_);
}

static int countDistributiveOrOverAnd(const node_t *root) {
  if (!root)
    return 0;
  int cnt = (isNodeTypeOp(root->type_) && root->type_ == node_type_or &&
             ((root->left_ && isNodeTypeOp(root->left_->type_) &&
               root->left_->type_ == node_type_and) ||
              ((root->right_ && isNodeTypeOp(root->right_->type_) &&
                root->right_->type_ == node_type_and))));
  return cnt + countDistributiveOrOverAnd(root->left_) +
         countDistributiveOrOverAnd(root->right_);
}

static void printASTStats(node_t *root) {
  fprintf(outfd, "Total Operators: %d\n", countOpNode(root));
  fprintf(outfd, "Total Operators(<->): %d\n",
          countOpTypeNode(root, node_type_bi_implication));
  fprintf(outfd, "Total Operators(->): %d\n",
          countOpTypeNode(root, node_type_implication));
  fprintf(outfd, "Total Operators(&): %d\n",
          countOpTypeNode(root, node_type_and));
  fprintf(outfd, "Total Operators(|): %d\n",
          countOpTypeNode(root, node_type_or));
  fprintf(outfd, "Total Operators(!): %d\n",
          countOpTypeNode(root, node_type_not));
  fprintf(outfd, "Total variables: %d\n", countOpTypeNode(root, node_type_var));
  // TODO
  fprintf(outfd, "Total variables(unique): %dTODO\n",
          countOpTypeNode(root, node_type_var));
}

static void printAST(node_t *node) {
  fprintf(outfd, "--------------------------------------\n");
  int n = countASTNode(node);
  fprintf(outfd, "#Nodes: %d\n", n);
  fprintf(outfd, "[\n");
  node_t *even[n], *odd[n];
  int startEven = 0, startOdd = 0, endEven = 0, endOdd = 0, level = 0;
  if (node) {
    even[startEven] = node;
    endEven = 1;
  }
  node_t **curr = NULL, **other = NULL;
  int *startCurr = NULL, *startOther = NULL;
  int *endCurr = NULL, *endOther = NULL;
  while (1) {
    if ((level & 1) == 0) {
      curr = even;
      other = odd;
      startCurr = &startEven;
      startOther = &startOdd;
      endCurr = &endEven;
      endOther = &endOdd;
    } else {
      curr = odd;
      other = even;
      startCurr = &startOdd;
      startOther = &startEven;
      endCurr = &endOdd;
      endOther = &endEven;
    }

    if (*startCurr == *endCurr)
      break;
    fprintf(outfd, "[");
    while (*startCurr < *endCurr) {
      node_t *t = curr[(*startCurr)++];
      printASTNode(t);
      if (t != NULL) {
        other[(*endOther)++] = t->left_;
        other[(*endOther)++] = t->right_;
      }
    }
    fprintf(outfd, " ]\n");
    level++;
  }
  fprintf(outfd, "]\n");
  fprintf(outfd, "--------------------------------------\n");
}

static void dumpUtil(node_t *root) {
  if (!root)
    return;
  if (root->type_ == node_type_var) {
    fprintf(outfd, "%s", root->var_name_);
    assert(root->left_ == NULL && root->right_ == NULL);
    return;
  }

  // print unary op
  if (root->type_ == node_type_not) {
    fprintf(outfd, "%s", opToStr[root->type_]);
  }

  if (root->left_) {
    if (isNodeTypeOp(root->left_->type_)) {
      fprintf(outfd, "(");
    }
  }

  dumpUtil(root->left_);

  if (root->left_) {
    if (isNodeTypeOp(root->left_->type_)) {
      fprintf(outfd, ")");
    }
  }
  // print binary the operator
  if (root->type_ != node_type_not) {
    fprintf(outfd, " %s ", opToStr[root->type_]);
  }
  if (root->right_) {
    if (isNodeTypeOp(root->right_->type_)) {
      fprintf(outfd, "(");
    }
  }
  dumpUtil(root->right_);
  if (root->right_) {
    if (isNodeTypeOp(root->right_->type_)) {
      fprintf(outfd, ")");
    }
  }
}
static void dumpAST(node_t *root) {
  fprintf(outfd, "(");
  dumpUtil(root);
  fprintf(outfd, ")\n");
}

static void dumpASTMinimalUtil(node_t *root, node_t *last) {
  if (!root)
    return;
  if (root->type_ == node_type_var) {
    fprintf(outfd, "%s", root->var_name_);
    assert(root->left_ == NULL && root->right_ == NULL);
    return;
  }

  // print unary op
  if (root->type_ == node_type_not) {
    fprintf(outfd, "%s", opToStr[root->type_]);
  }

  if (root->left_) {
    if (isNodeTypeOp(root->left_->type_)) {
      if (root->type_ != root->left_->type_) {
        fprintf(outfd, "(");
      }
    }
  }

  dumpASTMinimalUtil(root->left_, root);

  if (root->left_) {
    if (isNodeTypeOp(root->left_->type_)) {
      if (root->type_ != root->left_->type_) {
        fprintf(outfd, ")");
      }
    }
  }
  // print binary the operator
  if (root->type_ != node_type_not) {
    fprintf(outfd, " %s ", opToStr[root->type_]);
  }
  if (root->right_) {
    if (isNodeTypeOp(root->right_->type_)) {
      if (root->type_ != root->right_->type_) {
        fprintf(outfd, "(");
      }
    }
  }
  dumpASTMinimalUtil(root->right_, root);
  if (root->right_) {
    if (isNodeTypeOp(root->right_->type_)) {
      if (root->type_ != root->right_->type_) {
        fprintf(outfd, ")");
      }
    }
  }
}

static void dumpASTMinimal(node_t *root) {
  fprintf(outfd, "[");
  dumpASTMinimalUtil(root, NULL);
  fprintf(outfd, "]\n");
}
extern node_t *root;

#endif
