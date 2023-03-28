#ifndef _SYM_TABLE_H
#define _SYM_TABLE_H

#ifndef SYM_TABLE_P
#define SYM_TABLE_P 47

#endif

#ifndef SYM_TABLE_SZ
#define SYM_TABLE_SZ 701

#endif

#include "outils.h"

typedef struct sym_table_t sym_table_t;
typedef struct sym_table_entry_t ste_t;

struct sym_table_t {
    ste_t * e; /* entries */
    u64 sz;
    u64 c;
};

struct sym_table_entry_t {
    u64               h; /*  hash */
    char            * n; /*  name */
    void            * v; /* value */
    char            * t; /* type  */
    freefux_t * freefux; /* free  */
};

struct st_allocator {
    allocfux_t * alloc;
    freefux_t  * free;
};

ste_t nulle = {-1, '\0', NULL};

sym_table_t * st_create(u64 size);
void          st_destroy(sym_table_t *);
ste_t       * st_probe(sym_table_t *, char *);
void        * st_insert(sym_table_t *, char *, void *, char *, freefux_t *);
void        * st_delete(sym_table_t *, const char *);

u64 compute_hash(const char *, u64); // CONSIDER USE u64 compute_hash(const char * s, u64 table_sz) { ... }

#endif