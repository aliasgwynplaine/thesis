%{

#include "sym_table.h"
#include "pol_parser.h"
#include "pol.h"

#ifdef _DEBUG
int yydebug = 1;
#endif

extern sym_table_t * st;
extern pp_ctx_t   * ctx;
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


%token AAPOLTOK LLPOLTOK 
%token SYMTABTOK GETVARSTOK SETORDTOK SETVARSTOK F4TOK 
%token NEWLINE QUIT
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
    | stmts NEWLINE { printf("prelude> "); }
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
    : expression { $$ = create_set_for_expr_l(); set_insert($$, $1, nvars); ee_print($1); FREE($1->t); FREE($1); }
    | expression_list ',' expression 
        { $$ = $1; set_insert($$, $3, nvars); printf("expr_list! and "); ee_print($3); FREE($3->t); FREE($3); }
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
        //st_insert(st, "auxpol", aux_pol, "aapol");
    }
    ;

llpol_expr: LLPOLTOK '(' pol ')' { 
        $$ = malloc(sizeof(*$$)); 
        $$->t = strdup("aapol");  // todo: finish llpol implementation
        $$->v = (void *)aux_pol; 
        aux_pol = aapol_create(nvars);
        //st_insert(st, "auxpol", aux_pol, "aapol");
    }
    ;

pol: pol term
    | term
    | /* empty */
    ;

term: number '*' mvar { generate_term(aux_pol, $1, ctx); }
    | mvar '*' number { generate_term(aux_pol, $3, ctx); }
    | sign mvar       { generate_term(aux_pol, $1, ctx); }
    | mvar            { generate_term(aux_pol,  1, ctx); }
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

varx: VAR     { ctx->var_cntr[str_varlist_lookup($1, ctx->var_lst, ctx->nvars)]++; FREE($1); }
    | VAR exp { ctx->var_cntr[str_varlist_lookup($1, ctx->var_lst, ctx->nvars)]+= $2; FREE($1); }
    ;

exp: '^' INTEGER { $$ = $2; }
    ;

sign: '+' { $$ =  1; }
    | '-' { $$ = -1; }
    ;

directive: SYMTABTOK { print_sym_table(st); }
    | GETVARSTOK { printf("vars: "); print_lstr(ctx->var_lst); }
    | SETORDTOK termorder { printf("not implemented: %s order\n", $2); FREE($2); }
    | SETVARSTOK '{' vars '}' { /* update ctx->nvars and ctx->var_lst. export and delete accs */ }
    | F4TOK '(' expression_list ')' { printf("F4!!!\n"); /* set_print($3);*/} // here comes a set
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
