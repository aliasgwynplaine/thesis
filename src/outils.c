#include "outils.h"

void settimespec2zero(struct timespec * t) {
    t->tv_nsec = 0;
    t->tv_sec  = 0;
}

void settictac2zero(tictac_t * t) {
    settimespec2zero(&t->cmp_exp);
    settimespec2zero(&t->cpy_exp);
    settimespec2zero(&t->ops_exp);
    settimespec2zero(&t->ops_set);
    settimespec2zero(&t->total);
    settimespec2zero(&t->rref);

}

void print_tictac(tictac_t * t) {
    printf("cmp exp: %10jd.%09ld\n",t->cmp_exp.tv_sec, t->cmp_exp.tv_nsec);
    printf("cpy exp: %10jd.%09ld\n",t->cpy_exp.tv_sec, t->cpy_exp.tv_nsec);
    printf("ops exp: %10jd.%09ld\n",t->ops_exp.tv_sec, t->ops_exp.tv_nsec);
    printf("ops set: %10jd.%09ld\n",t->ops_set.tv_sec, t->ops_set.tv_nsec);
    printf("rref:    %10jd.%09ld\n",t->rref.tv_sec, t->rref.tv_nsec);
    printf("total:   %10jd.%09ld\n",t->total.tv_sec, t->total.tv_nsec);
}

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
