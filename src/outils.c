#include "outils.h"

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