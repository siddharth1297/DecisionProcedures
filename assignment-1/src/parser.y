%{
#include <assert.h>
#include <stdio.h>

#include "ast.h"

void yyerror(char *s);
int yylex();
extern char *yytext;

%}


%union {
  int val;
  struct node_t *node;
}

%token OPEN_PAR
%token CLOSE_PAR
%token<val> TRUE FALSE
%token<var_name> VAR

// Lowest precedence comes first

%right BI_IMP
%right IMP
%left OR
%left AND
%nonassoc NOT

%type <node> exp formula
%start exp

%%
exp : formula {
  root = $1;
  fprintf(outfd, "---AST created--- %p\n", root);
  return 0;
};

formula
: OPEN_PAR formula CLOSE_PAR { $$ = $2; }
| formula BI_IMP formula {$$ = create_node(node_type_bi_implication, "", 0, 0, $1, $3);}
| formula IMP formula {$$ = create_node(node_type_implication, "", 0, 0, $1, $3);}
| formula AND formula { $$ = create_node(node_type_and, "", 0, 0, $1, $3); }
| formula OR formula { $$ = create_node(node_type_or, "", 0, 0, $1, $3); }
| NOT formula { $$ = create_node(node_type_not, "", 0, 0, $2, NULL); }
| VAR {$$ = create_node(node_type_var, yytext, 0, 0, NULL, NULL); }
| TRUE { $$ = create_node(node_type_var, "T", 1, 1, NULL, NULL); }
| FALSE { $$ = create_node(node_type_var, "F", 1, 0, NULL, NULL); };
;
%%


void yyerror(char *s) {
  fprintf(outfd, "ERR: %s\n", s);
  exit(EXIT_FAILURE);
}

int yywrap(void) { return 1; }
