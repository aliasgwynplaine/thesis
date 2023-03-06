#ifndef _CORE_H
#define _CORE_H

#ifndef CORE_P
#define CORE_P 47

#endif

#ifndef CORE_MOD
#define CORE_MOD 701

#endif

#include "outils.h"

typedef struct sym_table_t sym_table_t;
typedef struct sym_table_entry_t ste_t;

struct sym_table_t {
    ste_t e[CORE_MOD]; /* entries */
    u64 sz;
};

struct sym_table_entry_t {
    char * n; /*  name */
    void * v; /* value */
};


u64 compute_hash(const char * s) {
    int pos = 0;
    u64 h = 0;
    u64 ppow = 1;
}


#endif