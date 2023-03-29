%{

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
int yydebug = 1;
#endif

%}

%union {
    float   float_val;
    int     int_val;
    char  * str_val;
}

%token INTEGER FLOATING TOKPOL NEWLINE VAR

%%

stmts: stmts stmt
    |
    ;

stmt: pol NEWLINE { printf("pol detected!\n"); }

pol: pol term
    | term
    |
    ;

term: coef '*' mvar
    | coef
    | mvar
    ;

coef: sign INTEGER
    | sign FLOATING
    | INTEGER
    | FLOATING
    ;

mvar: mvar '*' varx
    | sign varx
    | varx
    ;

varx: VAR
    | VAR exp
    ;

exp: '^' INTEGER
    ;

sign: '+'
    | '-'
    ;

%%


int yyerror(const char * str) {
    fprintf(stdout, "error: %s\n", str);

    return 0;
}


int yywrap() {
    return 1;
}
