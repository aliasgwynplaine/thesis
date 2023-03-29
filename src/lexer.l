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
floating    {digit}+\.{digit}+



%%

{integer}       return INTEGER;
{floating}      return FLOATING;
aapol           printf("AAPOLTOK");
llpol           printf("LLPOLTOK");
[a-z][0-9]*     return VAR;
\(              printf("OBRACE ");
\)              printf("EBRACE ");
\{              printf("OBRACKET ");
\}              printf("EBRACKET ");
\n              return NEWLINE;
[ \t]+          /* ignore */;
.               return (int) yytext[0];


%%