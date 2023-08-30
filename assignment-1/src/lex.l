%{
#include <stdio.h>
#include <unistd.h>
#include "ast.h"
#include "parser.tab.h"

%}

%%
"!"                     {return NOT;}
"&"                     {return AND;}
"|"                     {return OR;}
"->"                    {return IMP;}
"<->"                   {return BI_IMP;}
"("                     {return OPEN_PAR;}
")"                     {return CLOSE_PAR;}
"T"                     {return TRUE;}
"F"                     {return FALSE;}
[a-z][0-9]*             {return VAR;}
[ \t\n]                 {;}
.                       {
                            fprintf(outfd, "Unrecognised Character %s\n", yytext);
                            exit(EXIT_FAILURE);
                        }
%%
