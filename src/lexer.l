%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"

%}

digit       [0-9]
dnz         [1-9]
upper_alpha [A-Z]+
lower_alpha [a-z]

%%

[0-9]+      yylval.int_val = atoi(yytext); return NUMBER;
lower_alpha *(yylval.str_val) = yytext[0]; return ALPH;
\n          /* ignore */;
[ \t]+      /* ignore */;
.           return (int) yytext[0];       

%%