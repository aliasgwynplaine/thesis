#ifndef _OUTILS_H
#define _OUTILS_H

#include <stdio.h>

void print_lstr(char ** lstr)  {
    if (!lstr) {printf("NULLL");return;}
    printf("not null");

    int pos = 0;
    char ** ptr = lstr;

    while (*(ptr + pos) != NULL) {
        printf("%s ", *(ptr+pos));
        pos++;
    }

    printf("\n");
}

#endif