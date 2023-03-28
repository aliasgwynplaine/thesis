%{

#include <stdio.h>
#include <stdlib.h>


int yyerror(const char * str) {
    fprintf(stdout, "error: %s\n", str);

    return 0;
}


int yywrap() {
    return 1;
}


%}

%union {
    float   float_val;
    int     int_val;
    char  * str_val;
}

%token NUMBER TOKPOL ALPH


%%

stmts: stmts stmt ';'
    |
    ;

stmt: pol
    { printf("pol detected!\n"); }
    ; // todo: change

pol: term
    | sign term
    ;

term: coef
    | coef mvar
    ;

coef: num;

mvar: varx
    | varx '*' varx
    ;

varx: var
    | var exp
    ;

var: ALPH num;

exp: '^' num;

num: NUMBER;

sign: '+'
    | '-'
    ;

%%
