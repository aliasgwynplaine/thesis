%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
// {integer}       printf("INTEGER ");
// {floating}      printf("FLOATING ");
// aapol           printf("AAPOLTOK");
// llpol           printf("LLPOLTOK");
// [a-z][0-9]*     printf("VAR ");
// \*              printf("TIMES ");
// \+|\-           printf("SIGN ");
// \^              printf("POW ");
// \(              printf("OPAR ");
// \)              printf("EPAR ");
// \n              printf("\n");
// [ \t]+          /* ignore */

%}

digit       [0-9]
integer     {digit}+
floating    {digit}+\.{digit}{1,6}
variable    [a-zA-Z][a-zA-Z0-9_]*
string      \"(\\.|[^\"])*\"


%%

aapol               return AAPOLTOK;
llpol               return LLPOLTOK;
symtab              return SYMTABTOK;
set_vars            return SETVARSTOK;
quit|q|bye          return QUIT;
{integer}           yylval.int_val = atoi(yytext); return INTEGER;
{floating}          yylval.float_val = atof(yytext); return FLOATING;
{variable}          yylval.str_val = strdup(yytext); return VAR;
{string}            yylval.str_val = strdup(yytext); return STRING;
\n                  return NEWLINE;
[ \t]+              /* ignore */;
\/\/.*              /* ignore */;
\/\*(.*\n)*.*\*\/   /* ignore */;
.                   return (int) yytext[0];


%%