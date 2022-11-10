#include "outils.h"


/**
 * @brief prints a list of strings
 * as bytes
 * 
 * @param lstr 
 */
void print_lstrb(char ** lstr)  {
    if (!lstr) {return;}

    int pos = 0;
    char ** ptr = lstr;
    char * s;
    
    while (*(ptr + pos) != NULL) {
        s = *(ptr + pos);
        while (*s) {
            printf("%02x ", *s++);
        }
        pos++;
    }

    printf("\n");
}


/**
 * @brief prints a list of strings
 * as chars
 * 
 * @param lstr 
 */
void print_lstr(char ** lstr)  {
    if (!lstr) {printf("NULLL");return;}
    printf("not null\n");

    int pos = 0;
    char ** ptr = lstr;

    while (*(ptr + pos) != NULL) {
        printf("%s ", *(ptr+pos));
        pos++;
    }

    printf("\n");
}

int int_cumsum(int * p, int * c, int n) {
    if (!p || !c) return -1;
    
    int nz = 0, nz2 = 0;

    for (int i = 0; i < n; i++) {
        p[i] = nz;
        nz  += c[i];
        nz2 += c[i];
        c[i] = p[i];
    }

    p[n] = nz;

    return nz2;
}
