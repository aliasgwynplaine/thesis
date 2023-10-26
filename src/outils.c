#include "outils.h"

int  is_str_in_lstr(char * str, char ** lstr) {
    char ** ptr = lstr;

    while (*ptr != NULL) {
        if (strcmp(str, *ptr) == 0) return true;
        ptr++;
    }

    return false;
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


/**
 * @brief prints a list of strings
 * as chars
 * 
 * @param lstr 
 */
void print_lstr(char ** lstr)  {
    if (!lstr) {printf("NULLL");return;}

    int pos = 0;
    char ** ptr = lstr;

    while (*(ptr + pos)) {
        printf("%s ", *(ptr+pos));
        pos++;
    }

    printf("\n");
}

double int_cumsum(int * p, int * c, int n) {
    if (!p || !c) return 0;
    
    int nz = 0;
    double nz2 = 0;

    for (int i = 0; i < n; i++) {
        p[i] = nz;
        nz  += c[i];
        nz2 += c[i];
        c[i] = p[i];
    }

    p[n] = nz;

    return nz2;
}


/**
 * @brief function to traverse an array and 
 * return the max element in the array
 * 
 * @param a array of ints
 * @param p lower bound of the array a
 * @param q upper bound of the array a
 * 
 * @return the the max element
*/
inline int int_max(int * a, int p, int q) {
    if (p > q) SAYNEXITWERROR("check your bounds!");
    int mx = a[p];
    
    for (int i = p ; i <= q; i++)
        mx = __max(mx, a[i]);
    
    return mx;
}
/**
 * @brief function to traverse an array and 
 * return the index of the max element in 
 * the array
 * 
 * @param a array of ints
 * @param p lower bound of the array a
 * @param q upper bound of the array a
 * 
 * @return the index of the max element
*/
inline int int_max_idx(int * a, int p, int q) { 
    if (p > q) SAYNEXITWERROR("check your bounds!"); 
    int mx_idx = p;

    for (int i = p + 1 ; i <= q; i++)
        mx_idx = a[mx_idx] > a[i] ? mx_idx : i;
     
    return mx_idx; 
}


inline u64 u64_max(u64 * a, int p, int q) { 
    if (p > q) SAYNEXITWERROR("check your bounds!"); 
    u64 mx = a[p]; 

    for (int i = p + 1; i <= q; i++) 
        mx = __max(mx, a[i]); 
    
    return mx; 
}

inline u64 u64_max_idx(u64 * a, int p, int q) { 
    if (p > q) SAYNEXITWERROR("check your bounds!"); 
    u64 mx_idx = p; 

    for (int i = p ; i <= q; i++)
        mx_idx = a[mx_idx] > a[i] ? mx_idx : i; 
    
    return mx_idx; 
}
