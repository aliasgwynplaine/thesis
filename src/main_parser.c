#include <stdio.h>
#include "sym_table.h"
#include "grammar.h"

int yyparse();

sym_table_t * st;
int nvars;
char ** var_lst;

int main(int argc, char * argv[]) {
    nvars = 3;
    printf("Creating symbol table...");
    st = st_create(SYM_TABLE_SZ);
    printf("done!\n");
    var_lst = malloc((nvars + 1) * sizeof(*var_lst));
    var_lst[0] = "x";
    var_lst[1] = "y";
    var_lst[2] = "z";
    printf("setvars = {x,y,z}\n");
    printf("prelude> ");
    yyparse();

    return 0;
}