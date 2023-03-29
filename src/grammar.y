%{

#include <stdio.h>
#include <stdlib.h>
#include "sym_table.h"

#ifdef _DEBUG
int yydebug = 1;
#endif

//sym_table_t * st = st_create(SYM_TABLE_SZ);

%}

%union {
    float   float_val;
    int     int_val;
    char  * str_val;
}

%type <str_val> VAR

%token AAPOLTOK LLPOLTOK SYMTABTOK INTEGER FLOATING NEWLINE VAR

%%

stmts: stmts stmt
    |
    ;

stmt: simple_stmt NEWLINE

simple_stmt: pol
    | assignment
    | directive
    ;

assignment: VAR '=' expression  { printf("Here you insert %s to the hashtable\n", $1); }
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

directive: SYMTABTOK 
    ; /* OTHER DIRECTIVES MAY BE NEEDED*/

%%


int yyerror(const char * str) {
    fprintf(stdout, "error: %s\n", str);

    return 0;
}


int yywrap() {
    return 1;
}
