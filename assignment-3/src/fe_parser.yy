%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {FE}
/**
 * bison 3.3.2 change
 * %define parser_class_name to this, updated
 * should work for previous bison versions as 
 * well. -jcb 24 Jan 2020
 */
%define api.parser.class {Parser}

%code requires{
   namespace FE {
      class Driver;
      class Scanner;
   }
    class Ast;
    class Node;
}

%parse-param { Scanner  &scanner  }
%parse-param { Driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "fe_driver.hpp"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token COMMA

// Lowest precedence comes first

%right BI_IMP
%right IMP
%left OR
%left AND
%right EQUAL NOT_EQUAL
%nonassoc OPEN_PAR CLOSE_PAR
%nonassoc <Node*> NOT
%token <Node*> VAR

//%type <Ast*> exp formula
%nterm <Node*> formula
%nterm <Node*> exp
%nterm <Node*> term
%nterm <Node*> atom
%nterm <std::vector<Node*>> args
%nterm <Node*> arg
%nterm <Node*> funcname

%locations


%%
exp : formula {
  driver.updateRoot($1);
  return 0;
};

formula
: formula AND formula {$$ = driver.onOperator(node_type_and, std::vector<Node*>{$1, $3});}
| NOT formula {$$ = driver.onOperator(node_type_not, std::vector<Node*>{$2});}
| OPEN_PAR formula CLOSE_PAR {$$ = $2;}
| atom {$$=$1;}

atom
: term EQUAL term {$$ = driver.onOperator(node_type_equal, std::vector<Node*>{$1, $3});}
| term NOT_EQUAL term {$$ = driver.onOperator(node_type_not_equal, std::vector<Node*>{$1, $3});}
| funcname OPEN_PAR args CLOSE_PAR {$$ = driver.onNewPredicate($1, $3);}

term
: VAR {$$ = driver.onNewVar(scanner.text);}
| funcname OPEN_PAR args CLOSE_PAR {$$ = driver.onNewFunction($1, $3);}

args
: arg {$$ = vector<Node*>{$1};}
| args COMMA arg {$1.push_back($3); $$ = $1;}

arg
: term {$$ = $1;} 

funcname
: VAR {$$ = driver.onNewFunction(scanner.text, vector<Node*>());}
;
%%

void 
FE::Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << std::endl;
   abort();
}
