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
// string      \"(\\.|[^\"])*\"
// {string}            yylval.str_val = strdup(yytext); return STRING;

%}

digit       [0-9]
integer     {digit}+
floating    {digit}+\.{digit}{1,6}
variable    [a-zA-Z][a-zA-Z0-9_]*


%%

aapol               return AAPOLTOK;
pol                 return LLPOLTOK;
F4                  return F4TOK;
sort                return SORTTOK;
symtab              return SYMTABTOK;
monord              return MONORDTOK;
getvars             return GETVARSTOK;
setvars             return SETVARSTOK;
setord              return SETORDTOK;
export              return EXPORTTOK;
lex                 return LEX;
revlex              return RLEX;
glex                return GLEX;
grevlex             return GRLEX;
quit|q|bye          return QUIT;
{integer}           yylval.int_val = atoi(yytext);   return INTEGER;
{floating}          yylval.float_val = atof(yytext); return FLOATING;
{variable}          yylval.str_val = strdup(yytext); return VAR;
\n                  return NEWLINE;
[ \t]+              /* ignore */;
\/\/.*              /* ignore */;
\/\*(.*\n)*.*\*\/   /* ignore */;
.                   return (int) yytext[0];


%%