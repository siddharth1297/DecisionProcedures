%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexScanner */ 
#include "fe_scanner.hpp"
#undef  YY_DECL
#define YY_DECL int FE::Scanner::yylex( FE::Parser::semantic_type * const lval, FE::Parser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = FE::Parser::token;

/* define yyterminate as this instead of NULL */
//#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="MC::Scanner"
%option noyywrap
%option c++

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"!"                     {return token::NOT;}
"&"                     {return token::AND;}
"|"                     {std::cerr << " | NOT ALLOWED\n"; abort();}
"->"                    {std::cerr << " -> NOT ALLOWED\n"; abort();}
"<->"                   {std::cerr << " <-> NOT ALLOWED\n"; abort();}
"("                     {return token::OPEN_PAR;}
")"                     {return token::CLOSE_PAR;}
"="                     {return token::EQUAL;}
"!="                    {return token::NOT_EQUAL;}
","                     {return token::COMMA;}
[a-zA-Z0-9]+   {
               yylval->build< Node* >( nullptr);
               text = std::string(yytext);
               return( token::VAR );
            }
[ \t\n]     {}
.           {std::cerr << "Invalid char: " << yytext << std::endl; exit(EXIT_FAILURE);}
%%
