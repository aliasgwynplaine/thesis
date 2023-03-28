#include <stdio.h>
#include "grammar.h"

int yyparse();

int main(int argc, char * argv[]) {
    yyparse();

    return 0;
}