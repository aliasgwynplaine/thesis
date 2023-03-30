%{

#include <stdio.h>
#include <stdlib.h>
#include "sym_table.h"

#ifdef _DEBUG
int yydebug = 1;
#endif

extern sym_table_t * st;
extern int nvars;
extern char ** var_lst;

%}

%union {
    float         float_val;
    int           int_val;
    char        * str_val;
    struct expr_entry_t {
        char * t;
        void * v;
    } expr_val;
}


%token AAPOLTOK LLPOLTOK SYMTABTOK SETVARSTOK NEWLINE QUIT
%token <float_val> FLOATING
%token <int_val>   INTEGER
%token <str_val>   VAR
%token <str_val>   STRING
%type <expr_val>   expression
%type <expr_val>   aapol_expr;
%type <expr_val>   llpol_expr;
%type <expr_val>   number;
%type <int_val>    sign;

%%

stmts: stmts stmt
    |
    ;

stmt: simple_stmt NEWLINE { printf("prelude> "); }

simple_stmt: VAR { 
        ste_t * entry = st_probe(st, $1);

        if (!entry) printf("undefined variable: %s\n", $1);
        else {
            if (strcmp(entry->t, "aapol") == 0) aapol_print((aapol_t *)entry->v);
            if (strcmp(entry->t, "llpol") == 0) llpol_print((llpol_t *)entry->v);
            if (strcmp(entry->t, "int") == 0) printf("%d\n", *(int *)entry->v);
            if (strcmp(entry->t, "float") == 0) printf("%f\n", *(float *)entry->v);
        } 
    }
    | assignment
    | directive
    ;

assignment: VAR '=' expression  { 
        /*printf("Here you insert '%s' to the symboltable\n", $1);
        printf("type: %s", $3.t);

        if (strcmp($3.t, "aapol") == 0)
            aapol_print((aapol_t *)$3.v);

        if (strcmp($3.t, "llpol") == 0)
            llpol_print((llpol_t *)$3.v);*/
        
        st_insert(st, $1, $3.v, $3.t);
    }
    ;

expression: aapol_expr 
    | llpol_expr    
    | number
    ;

aapol_expr: AAPOLTOK '(' STRING ')' { 
        int len = strlen($3);
        $$.t = strdup("aapol");
        char * aapol_str = malloc((len - 2) * sizeof(*aapol_str));
        CHECKPTR(aapol_str);
        memcpy(aapol_str, $3 + 1, len - 2);
        aapol_str[len - 1] = 0;
        aapol_t * a = str2aapol(aapol_str, var_lst, nvars);
        if (!a) { 
            printf("wrong!!\n"); exit(1);
            //todo : handle
        }
        $$.v = (void *) a;
        FREE(aapol_str);
        FREE($3);
    }
    ;

llpol_expr: LLPOLTOK '(' STRING ')' {
        int len = strlen($3);
        $$.t = strdup("llpol");
        char * llpol_str = malloc((len - 2) * sizeof(*llpol_str));
        CHECKPTR(llpol_str);
        memcpy(llpol_str, $3 + 1, len - 2);
        llpol_str[len - 1] = 0;
        llpol_t * a = str2llpol(llpol_str, var_lst, nvars);
        if (!a) { 
            printf("wrong!!\n"); exit(1);
            //todo : handle
        }
        $$.v = (void *) a;
        FREE(llpol_str);
    }
    ;

number: sign INTEGER { $$.t = strdup("int"); $$.v = malloc(sizeof(int)); *(int *)$$.v = $1 * $2; }
    | sign FLOATING  { $$.t = strdup("float"); $$.v = malloc(sizeof(float)); *(float *)$$.v = $1 * $2; }
    | INTEGER        { $$.t = strdup("int"); $$.v = malloc(sizeof(int)); *(int *)$$.v = $1; }
    | FLOATING       { $$.t = strdup("float"); $$.v = malloc(sizeof(float)); *(float *)$$.v = $1; }
    ;

sign: '+' { $$ =  1; }
    | '-' { $$ = -1; }
    ;

directive: SYMTABTOK { print_sym_table(st); }
    | SETVARSTOK '{' vars '}'
    | QUIT { printf("bye!\n"); exit(0); }
    ; /* OTHER DIRECTIVES MAY BE NEEDED*/

vars: vars ',' VAR
    | VAR
    ;

%%


int yyerror(const char * str) {
    fprintf(stdout, "error: %s\n", str);

    return 0;
}


int yywrap() {
    return 1;
}
