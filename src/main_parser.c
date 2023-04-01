#include <stdio.h>
#include "sym_table.h"
#include "grammar.h"
#include "pol_parser.h"

int yyparse();

sym_table_t * st;
int nvars;
char ** var_lst;
u64 * var_cntr;
llpol_t * aux_pol;

int main(int argc, char * argv[]) {
    nvars = 3;
    var_cntr = calloc(nvars, sizeof(*var_cntr));
    printf("Creating symbol table...");
    st = st_create(SYM_TABLE_SZ);
    printf("done!\n");
    var_lst = malloc((nvars) * sizeof(*var_lst));
    var_lst[0] = "x"; //var_lst[0] = strdup("x");
    var_lst[1] = "y";
    var_lst[2] = "z";
    aux_pol = llpol_create(3);
    printf("auxpol before: %p\n", aux_pol);
    printf("setvars = {x,y,z}\n");
    printf("prelude> ");
    yyparse();
    printf("auxpol after: %p\n", aux_pol);
    printf(":P\n");
    //print_sym_table(st);
    st_destroy(st);
    llpol_free(aux_pol);
    FREE(var_cntr);
    FREE(var_lst);

    return 0;
}