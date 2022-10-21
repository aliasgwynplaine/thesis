#include "outils.h"

/**
 * @brief binary node struct.
 * Definition and functions from 
 * Corwen
 * 
 */
struct bnode_t {
    int      sz;
    bnode_t * l;
    bnode_t * r;
};

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