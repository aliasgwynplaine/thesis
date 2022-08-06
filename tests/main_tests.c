#include <stdio.h>
#include <stdlib.h>
#include "sparce.h"

int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <test.mat>\n", argv[0]);
        return 1;
    }

    FILE * f = fopen(argv[1], "r");
    cs * T = cs_load(f);
    cs * S = cs_load(fopen("./tests/test3.mat", "r"));
    printf("S: ");
    cs_print(S, 1);
    cs * Sc = cs_compress(S);
    printf("T: ");
    cs_print(T, 1);
    printf("\n");
    cs * C = cs_compress(T);
    printf("Compressed T -> C: ");
    cs_print(C, 0);
    printf("\n");
    cs * CT = cs_transpose(C, 1);
    cs_print(CT, 0);
    printf("\n");
    cs * CTT = cs_transpose(CT, 1);
    cs_print(CTT, 0);
    double v[4] = {1, 2, 3};
    double * res = cs_malloc(4, sizeof(double));
    int r = gapxpy(CT, v, res);

    for (int i = 0; i < 4; i++) printf(" %f ", res[i]);
    
    printf("\n*********---\n");
    double * res2 = cs_malloc(4, sizeof(double));
    r = gatxpy(C, v, res2);
    
    for (int i = 0; i < 4; i++) printf(" %f ", res2[i]);
    
    printf("\n");
    cs_free(res);
    printf("\n");

    cs * SplusC = cs_add(C, Sc, 1, 1);
    cs_print(SplusC, 1);
    printf("\n");

    return 0;
}