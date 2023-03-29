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

%token INTEGER FLOATING AAPOLTOK LLPOLTOK NEWLINE VAR

%%

stmts: stmts stmt
    |
    ;

stmt: simple_stmt NEWLINE

simple_stmt: pol
    | assignment
    ;

assignment: VAR '=' expression
    ;

expression: aapol_expr
    | llpol_expr
    | number
    ;

aapol_expr: AAPOLTOK '(' pol ')' { printf("aapol detected!\n"); }
    ;

llpol_expr: LLPOLTOK '(' pol ')' { printf("llpol detected!\n"); }
    ;

pol: pol term
    | term
    |
    ;

term: number '*' mvar
    | number
    | mvar
    ;

number: sign INTEGER
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
