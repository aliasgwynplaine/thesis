%{

#include "sym_table.h"
#include "tree.h"
#include "pol_parser.h"
#include "pol.h"

#ifdef _DEBUG
int yydebug = 1;
#endif

extern sym_table_t * st;
extern pp_ctx_t   * ctx;
extern aapol_t * aux_aapol;
extern llpol_t * aux_llpol;
extern set_t * _pol_acc_in;
extern set_t * _pol_acc_out;
extern mon_ord_str;
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
%token SYMTABTOK MONORDTOK GETVARSTOK SETORDTOK SETVARSTOK F4TOK SORTTOK
%token NEWLINE QUIT
%token <str_val>   LEX GLEX GRLEX RLEX;
%token <float_val> FLOATING
%token <int_val>   INTEGER
%token <str_val>   VAR
//%type <expr_l_val> expression_list;
%type <expr_val>   expression_list
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

stmts: stmts stmt NEWLINE { printf("%s> ", mon_ord_str); }
    | stmts NEWLINE { printf("%s> ", mon_ord_str); }
    | /* empty */
    | error NEWLINE { yyerror("Error!"); printf("%s> ", mon_ord_str); yyerrok; }
    ;

stmt: VAR { print_var(st, $1); printf("\n"); FREE($1); }
    | assignment
    | expression { ee_print($1); printf("\n"); ee_free($1); }
    | directive
    ;

assignment: VAR '=' expression
        { if ($3) { st_insert(st, $1, $3->v, $3->t); FREE($3); } }
    ;

expression_list
    : expression { 
        //$$ = create_set_for_expr_l(); set_insert($$, $1, nvars); ee_print($1); FREE($1->t); FREE($1);
        if (strcmp($1->t, "llpol") == 0) { rbtree_probe(_pol_acc_in, $1->v); }
        if (strcmp($1->t, "number") == 0) {
            llpol_t * pol = llpol_create(ctx->nvars);
            llpol_addterm(pol, *(float*)$1->v, 0, ctx->order);
            rbtree_probe(_pol_acc_in, pol);
            free($1->v);
        }
        free($1->t); free($1);
    }
    | expression_list ',' expression { 
        //$$ = $1; set_insert($$, $3, nvars); printf("expr_list! and "); ee_print($3); FREE($3->t); FREE($3);
        if (strcmp($3->t, "llpol") == 0) { rbtree_probe(_pol_acc_in, $3->v); }
        if (strcmp($3->t, "number") == 0) {
            llpol_t * pol = llpol_create(ctx->nvars);
            llpol_addterm(pol, *(float*)$3->v, 0, ctx->order);
            rbtree_probe(_pol_acc_in, pol);
            printf(".\n");
            free($3->v);
        }
        free($3->t); free($3);
    }
    ;

expression
    : expression '+' expression { $$ = resolve_op_expression(st, $1, $3, "+", ctx); ee_free($1); ee_free($3); }
    | expression '-' expression { $$ = resolve_op_expression(st, $1, $3, "-", ctx); ee_free($1); ee_free($3); }
    | expression '*' expression { $$ = resolve_op_expression(st, $1, $3, "*", ctx); ee_free($1); ee_free($3); }
    | expression '/' expression { $$ = resolve_op_expression(st, $1, $3, "/", ctx); ee_free($1); ee_free($3); }
    | pol_expr
    | number { $$ = resolve_number_as_expression(st, $1); }
    | VAR { $$ = resolve_var_as_expression(st, $1, ctx); FREE($1); }
    ;

pol_expr: aapol_expr | llpol_expr;

aapol_expr: AAPOLTOK '(' pol ')' {
        $$ = malloc(sizeof(*$$)); 
        $$->t = strdup("aapol"); 
        $$->v = (void *)aux_aapol; 
        aux_aapol = aapol_create(ctx->nvars);
        //st_insert(st, "auxpol", aux_aapol, "aapol");
    }
    ;

llpol_expr: LLPOLTOK '(' pol ')' { 
        $$ = malloc(sizeof(*$$)); 
        $$->t = strdup("llpol");  // todo: finish llpol implementation
        $$->v = (void *)aux_llpol; 
        aux_llpol = llpol_create(ctx->nvars);
        //st_insert(st, "auxpol", aux_llpol, "llpol");
    }
    ;

pol: pol term
    | term
    | /* empty */
    ;

term: number '*' mvar { generate_term(aux_llpol, $1, ctx); }
    | mvar '*' number { generate_term(aux_llpol, $3, ctx); }
    | sign mvar       { generate_term(aux_llpol, $1, ctx); }
    | mvar            { generate_term(aux_llpol,  1, ctx); }
    | number          { llpol_addterm(aux_llpol, $1, NULL, ctx->order);}
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
    | MONORDTOK { printf("%d\n", ctx->order); }
    | GETVARSTOK { printf("vars: "); print_lstr(ctx->var_lst); }
    | SETORDTOK termorder { change_mon_order(ctx, $2); FREE($2); FREE(mon_ord_str); mon_ord_str = get_mon_order_str(ctx);}
    | SETVARSTOK '{' vars '}' { /* update ctx->nvars and ctx->var_lst. export and delete accs */ }
    | SORTTOK VAR { ee_t * e = get_object_from_var(st, $2); aapol_sort(e->v); free(e->t); free(e); free($2); }
    | F4TOK '(' expression_list ')' { f4_wrapper(_pol_acc_in, _pol_acc_out, ctx); set_print(_pol_acc_out); printf("\n");}
    | QUIT { printf("bye!\n"); yylex_destroy(); return 0; }
    ; /* OTHER DIRECTIVES MAY BE NEEDED*/

termorder: LEX   { $$ = strdup("lex"); }
    | GLEX  { $$ = strdup("glex"); }
    | RLEX  { $$ = strdup("revlex"); }
    | GRLEX { $$ = strdup("grevlex"); }
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
