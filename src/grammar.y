%{

#include "sym_table.h"
#include "pol_parser.h"
#include "pol.h"

#ifdef _DEBUG
int yydebug = 1;
#endif

extern sym_table_t * st;
extern int nvars;
extern char ** var_lst;
extern u64 * var_cntr;
extern llpol_t * aux_pol;
// %token <str_val>   STRING

%}

%code requires { #include "pol_parser.h" }

%union {
    float         float_val;
    int           int_val;
    char        * str_val;
    ee_t        * expr_val;
}


%token AAPOLTOK LLPOLTOK SYMTABTOK SETVARSTOK NEWLINE QUIT
%token <float_val> FLOATING
%token <int_val>   INTEGER
%token <str_val>   VAR
%type <expr_val>   expression
%type <expr_val>   aapol_expr
%type <expr_val>   llpol_expr
%type <expr_val>   pol_expr
%type <float_val>  number
%type <int_val>    sign
%type <int_val>    exp

%left '+' '-'
%right '*' '/'
%right '='

%%

stmts: stmts stmt NEWLINE { printf("prelude> "); }
    | stmts NEWLINE
    | /* empty */
    | error NEWLINE { yyerror("Error!"); printf("prelude> "); yyerrok;  }
    ;

stmt: VAR { 
        ste_t * entry = st_probe(st, $1);

        if (!entry) printf("undefined variable: %s\n", $1);
        else {
            if (strcmp(entry->t, "aapol") == 0) aapol_print((aapol_t *)entry->v);
            if (strcmp(entry->t, "llpol") == 0) llpol_print((llpol_t *)entry->v);
            if (strcmp(entry->t, "number") == 0) printf("%f\n", *(float *)entry->v);
        } 
        FREE($1);
    }
    | assignment
    | directive
    ;

assignment: VAR '=' expression  { 
        printf("Here you insert '%s' to the symbol table\n", $1);
        if ($3->t) st_insert(st, $1, $3->v, $3->t);
        FREE($3);
    }
    ;

expression: expression '+' expression
    | expression '-' expression
    | expression '*' expression
    | expression '/' expression 
    | pol_expr
    | number { 
        //printf("num: %f\n", $1);
        // yylex_destroy(); return 0;
        $$ = malloc(sizeof(*$$)); CHECKPTR($$);
        $$->t = strdup("number");
        $$->v = malloc(sizeof(float)); CHECKPTR($$->v);
        *(float *)$$->v = $1;
        }
    | VAR { 
        ste_t * entry = st_probe(st, $1);
        if (!entry) {
            printf("undefined variable: %s\n", $1); $$->t = NULL;
        } else {
            $$ = malloc(sizeof(*$$));
            $$->t = entry->t;
            $$->v = entry->v;
        }
        FREE($1);
        }
    ;

pol_expr: aapol_expr | llpol_expr;

aapol_expr: AAPOLTOK '(' pol ')' { 
    printf("NOT IMPLEMENTED. Using llpol.\n"); 
    $$ = malloc(sizeof(*$$)); 
    $$->t = strdup("llpol"); 
    $$->v = (void *)aux_pol; 
    aux_pol = llpol_create(nvars);
    }
    ;

llpol_expr: LLPOLTOK '(' pol ')' { 
    printf("llpol detected!\n"); 
    $$ = malloc(sizeof(*$$)); 
    $$->t = strdup("llpol"); 
    $$->v = (void *)aux_pol; 
    aux_pol = llpol_create(nvars);
    }
    ;

pol: pol term
    | term
    | /* empty */
    ;

term: number '*' mvar { ee_generate_term(aux_pol, $1, var_cntr, var_lst, nvars); }
    | mvar '*' number { ee_generate_term(aux_pol, $3, var_cntr, var_lst, nvars); }
    | sign mvar       { ee_generate_term(aux_pol, $1, var_cntr, var_lst, nvars); }
    | mvar            { ee_generate_term(aux_pol, 1, var_cntr, var_lst, nvars);  }
    | number          { llpol_addterm(aux_pol, 1, 0);}
    ;

number: sign INTEGER { $$ = (float) ($1 * $2); }
    | sign FLOATING  { $$ = $1 * $2; }
    | INTEGER        { $$ = (float) $1; }
    | FLOATING       { $$ = $1; }
    ;

mvar: mvar '*' varx
    | varx
    ;

varx: VAR     { int idx = str_varlist_lookup($1, var_lst, nvars); var_cntr[idx]++; FREE($1); }
    | VAR exp { int idx = str_varlist_lookup($1, var_lst, nvars); var_cntr[idx]+= $2; FREE($1); }
    ;

exp: '^' INTEGER { $$ = $2; }
    ;

sign: '+' { $$ =  1; }
    | '-' { $$ = -1; }
    ;

directive: SYMTABTOK { print_sym_table(st); }
    | SETVARSTOK '{' vars '}' { /* update nvars and var_lst */ }
    | QUIT { printf("bye!\n"); yylex_destroy(); return 0; }
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
