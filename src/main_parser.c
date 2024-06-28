#include <stdio.h>
#include "sym_table.h"
#include "grammar.h"
#include "pol_parser.h"

int yyparse();

sym_table_t * st;
pp_ctx_t * ctx;
tictac_t * tictac;
aapol_t * aux_aapol;
llpol_t * aux_llpol;
set_t * _pol_acc_in;
set_t * _pol_acc_out;
set_t * _vars;
char  * mon_ord_str;
i32 p;
extern rbt_cmpfux_t aapol_monomial_cmp_wrap;
extern rbt_cmpfux_t pol_monomial_cmp_wrap;

int main(int argc, char * argv[]) {
    ctx = malloc(sizeof(*ctx));
    ctx->order = grevlex;
    mon_ord_str = strdup("grevlex");
    p = 65521;
    ctx->nvars = 4;
    ctx->var_cntr = calloc(ctx->nvars, sizeof(*(ctx->var_cntr)));
    printf("Creating symbol table...");
    _vars        = rbtree_create(strcmp, NULL, NULL);
    _pol_acc_in  = rbtree_create(pol_monomial_cmp_wrap, &(ctx->order), NULL);
    _pol_acc_out = rbtree_create(pol_monomial_cmp_wrap, &(ctx->order), NULL);
    st = st_create(SYM_TABLE_SZ);
    printf("done!\n");
    ctx->var_lst = malloc((ctx->nvars + 1) * sizeof(*(ctx->var_lst)));
    ctx->var_lst[0] = strdup("a"); //var_lst[0] = strdup("x");
    ctx->var_lst[1] = strdup("b");
    ctx->var_lst[2] = strdup("c");
    ctx->var_lst[3] = strdup("d");
    ctx->var_lst[4] = NULL;
    aux_aapol = aapol_create(ctx->nvars);
    aux_llpol = llpol_create(ctx->nvars);
    st_insert(st, strdup("_pol_acc_in"), _pol_acc_in, strdup("acc"));
    st_insert(st, strdup("_pol_acc_out"), _pol_acc_out, strdup("acc"));
    printf("aux_aapol before: %p\n", aux_aapol);
    printf("aux_llpol before: %p\n", aux_llpol);
    printf("setvars = {a, b, c, d}\n");
    printf("%s> ", mon_ord_str);
    yyparse();
    printf("aux_aapol after: %p\n", aux_aapol);
    printf("aux_llpol after: %p\n", aux_llpol);
    printf(":P\n");
    //print_sym_table(st);
    rbtree_destroy(_vars, free);
    st_destroy(st);
    aapol_free(aux_aapol);
    llpol_free(aux_llpol);
    free(mon_ord_str);
    for (int i = 0; i < ctx->nvars; i++) free(ctx->var_lst[i]);
    free(ctx->var_cntr);
    free(ctx->var_lst);
    free(ctx);

    return 0;
}