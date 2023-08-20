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
extern aapol_t * aux_pol;
extern cmpfux_t * cfux;
// %token <str_val>   STRING

%}

%code requires { #include "pol_parser.h" }

%union {
    float         float_val;
    int           int_val;
    char        * str_val;
    ee_t        * expr_val;
    rbtree_t    * expr_l_val;
}


%token AAPOLTOK LLPOLTOK SYMTABTOK SETORDTOK SETVARSTOK F4TOK NEWLINE QUIT
%token <str_val>   LEX GLEX GRLEX RLEX;
%token <float_val> FLOATING
%token <int_val>   INTEGER
%token <str_val>   VAR
%type <expr_l_val> expression_list; 
%type <expr_val>   expression
%type <expr_val>   aapol_expr
%type <expr_val>   llpol_expr
%type <expr_val>   pol_expr
%type <str_val>    termorder
%type <float_val>  number
%type <int_val>    sign
%type <int_val>    exp

%destructor { FREE($$); } VAR;

%left '+' '-'
%left '*' '/'
%right '='

%%

stmts: stmts stmt NEWLINE { printf("prelude> "); }
    | stmts NEWLINE
    | /* empty */
    | error NEWLINE { yyerror("Error!"); printf("prelude> "); yyerrok; }
    ;

stmt: VAR { print_var(st, $1); FREE($1); }
    | assignment
    | expression { ee_print($1); ee_free($1); }
    | directive
    ;

assignment: VAR '=' expression
        { if ($3) { st_insert(st, $1, $3->v, $3->t); FREE($3); } }
    ;

expression_list
    : expression { printf("expr: "); ee_print($1); ee_free($1); }
    | expression_list ',' expression { printf("expr_list! and "); ee_print($3); ee_free($3); }
    ;

expression
    : expression '+' expression { $$ = resolve_op_expression(st, $1, $3, "+"); ee_free($1); ee_free($3); }
    | expression '-' expression { $$ = resolve_op_expression(st, $1, $3, "-"); ee_free($1); ee_free($3); }
    | expression '*' expression { $$ = resolve_op_expression(st, $1, $3, "*"); ee_free($1); ee_free($3); }
    | expression '/' expression { $$ = resolve_op_expression(st, $1, $3, "/"); ee_free($1); ee_free($3); }
    | pol_expr
    | number { $$ = resolve_number_as_expression(st, $1); }
    | VAR { $$ = resolve_var_as_expression(st, $1); FREE($1); }
    ;

pol_expr: aapol_expr | llpol_expr;

aapol_expr: AAPOLTOK '(' pol ')' {
        $$ = malloc(sizeof(*$$)); 
        $$->t = strdup("aapol"); 
        $$->v = (void *)aux_pol; 
        aux_pol = aapol_create(nvars);
    }
    ;

llpol_expr: LLPOLTOK '(' pol ')' { 
        $$ = malloc(sizeof(*$$)); 
        $$->t = strdup("aapol");  // todo: finish llpol implementation
        $$->v = (void *)aux_pol; 
        aux_pol = aapol_create(nvars);
    }
    ;

pol: pol term
    | term
    | /* empty */
    ;

term: number '*' mvar { generate_term(aux_pol, $1, var_cntr, var_lst, nvars); }
    | mvar '*' number { generate_term(aux_pol, $3, var_cntr, var_lst, nvars); }
    | sign mvar       { generate_term(aux_pol, $1, var_cntr, var_lst, nvars); }
    | mvar            { generate_term(aux_pol, 1, var_cntr, var_lst, nvars);  }
    | number          { aapol_addterm(aux_pol, $1, 0);}
    ;

number: sign INTEGER { $$ = (float) ($1 * $2); }
    | sign FLOATING  { $$ = $1 * $2; }
    | INTEGER        { $$ = (float) $1; }
    | FLOATING       { $$ = $1; }
    ;

mvar: mvar '*' varx
    | varx
    ;

varx: VAR     { var_cntr[str_varlist_lookup($1, var_lst, nvars)]++; FREE($1); }
    | VAR exp { var_cntr[str_varlist_lookup($1, var_lst, nvars)]+= $2; FREE($1); }
    ;

exp: '^' INTEGER { $$ = $2; }
    ;

sign: '+' { $$ =  1; }
    | '-' { $$ = -1; }
    ;

directive: SYMTABTOK { print_sym_table(st); }
    | SETORDTOK termorder { printf("not implemented: %s order\n", $2); FREE($2); }
    | SETVARSTOK '{' vars '}' { /* update nvars and var_lst */ }
    | F4TOK '(' expression_list ')' { printf("F4!!!\n"); } // here comes a set
    | QUIT { printf("bye!\n"); yylex_destroy(); return 0; }
    ; /* OTHER DIRECTIVES MAY BE NEEDED*/

termorder: LEX   { $$ = strdup("lex"); }
    | GLEX  { $$ = strdup("glex"); }
    | RLEX  { $$ = strdup("rlex"); }
    | GRLEX { $$ = strdup("grlex"); }
    ;

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
