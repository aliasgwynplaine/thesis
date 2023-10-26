#include <stdio.h>
#include "sym_table.h"
#include "grammar.h"
#include "pol_parser.h"

int yyparse();

sym_table_t * st;
int nvars;
char ** var_lst;
u64 * var_cntr;
aapol_t * aux_pol;
rbt_cmpfux_t * cfux;
extern rbt_cmpfux_t aapol_monomial_cmp_wrap;

int main(int argc, char * argv[]) {
    nvars = 4;
    cfux  = aapol_monomial_cmp_wrap;
    var_cntr = calloc(nvars, sizeof(*var_cntr));
    
    printf("Creating symbol table...");
    st = st_create(SYM_TABLE_SZ);
    printf("done!\n");
    var_lst = malloc((nvars+1) * sizeof(*var_lst));
    var_lst[0] = "a"; //var_lst[0] = strdup("x");
    var_lst[1] = "b";
    var_lst[2] = "c";
    var_lst[3] = "d";
    var_lst[4] = NULL;
    aux_pol = aapol_create(nvars);
    //st_insert(st, strdup("auxpol"), aux_pol, strdup("aapol"));
    printf("auxpol before: %p\n", aux_pol);
    printf("setvars = {a, b, c, d}\n");
    printf("prelude> ");
    yyparse();
    printf("auxpol after: %p\n", aux_pol);
    printf(":P\n");
    //print_sym_table(st);
    st_destroy(st);
    aapol_free(aux_pol);
    FREE(var_cntr);
    FREE(var_lst);

    return 0;
}