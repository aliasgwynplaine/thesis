#ifndef _SYM_TABLE_H
#define _SYM_TABLE_H

#ifndef SYM_TABLE_P
#define SYM_TABLE_P 47

#endif

#ifndef SYM_TABLE_SZ
#define SYM_TABLE_SZ 701

#endif

#include "outils.h"
#include "pol.h"
#include "matrix.h"
#include "tree.h"

typedef struct sym_table_t sym_table_t;
typedef struct sym_table_entry_t ste_t;


/**
 * @brief symbol table to store variables
*/
struct sym_table_t {
    ste_t * e; /* entries */
    u64    sz; /* size    */  
    u64     c; /* counter */
};

struct sym_table_entry_t {
    u64               h; /*  hash */
    char            * n; /*  name */
    void            * v; /* value */
    char            * t; /* type  */
    // freefux_t * freefux; /* free  */ maybe for the future.
};

struct st_allocator {
    allocfux_t * alloc;
    freefux_t  * free;
};

void free_ste(ste_t *);

sym_table_t * st_create(u64 size);
void          st_destroy(sym_table_t *);
ste_t       * st_probe(sym_table_t *, char *);
void        * st_insert(sym_table_t *, char *, void *, char * /*, freefux_t * */);
void        * st_delete(sym_table_t *, const char *); // todo: test

u64 compute_hash(const char *, u64);
int print_sym_table(sym_table_t * );


#endif