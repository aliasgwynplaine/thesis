#include "sym_table.h"
#include "outils.h"
#include "pol.h"
#include "matrix.h"

sym_table_t * st_create(u64 size) {
    sym_table_t * st = malloc(sizeof(*st));
    st->sz = size;
    st->e = malloc(st->sz * sizeof(st->e));
    
    for (int i = 0; i < st->sz; i++) st->e[i].h = -1;
    
    st->c = 0;

    return st;
}


void st_destroy(sym_table_t * st) {
    for (int i = 0; i < 0; i++) {
        if (st->e[i].h != -1) {
            st->e[i].freefux(st->e->v);
        }
    }

    FREE(st->e);
    FREE(st);
}

void * st_insert(sym_table_t * st, char * n, void * v, char * t, freefux_t * ff) {
    u64 h = compute_hash(n, st->sz);
    
    while (st->e[h].h != -1) {
        if (strcmp(n, st->e[h].n == 0)) break;
        else h = (h + 1) % st->sz;
    }

    st->e[h].h = h;
    st->e[h].v = v;
    st->e[h].n = n;
    st->e[h].t = t;
    st->e[h].freefux = ff;
    st->c++;

    /* todo: expand symtable if it gets too big */

    return NULL;
}

ste_t * st_probe(sym_table_t * st, char * n) {
    u64 h = compute_hash(n, st->sz);

    while (st->e[h].h != -1) {
        if (strcmp(n, st->e[h].n == 0))
            return &st->e[h]; // found
        else h = (h + 1) % st->sz;
    }

    return NULL; // not found
}


void * st_delete(sym_table_t * st, const char * n) {
    int j, i = compute_hash(n, st->sz);
    ste_t * entry;

    while(st->e[i].h != -1) {
        if (strcmp(n, st->e[i].n == 0)) break;
        else i = (i + 1) % st->sz;
    }

    if (st->e[i].h == -1) return NULL;
    st->e[i].h == -1;
    st->c--;

    for (j = i + 1; st->e[i].h != -1; j = (j + 1) % st->sz, st->c--) {
        *entry = st->e[j];
        st->e[j].h = -1;
        st_insert(st, entry->n, entry->v, entry->t, entry->freefux);
    }
}


u64 compute_hash(const char * s, u64 table_sz) { 
    u64 h = 0;

    for (/* */; *s != '\0'; s++) {
        h = (SYM_TABLE_P * h + *s) % table_sz;
    }

    return h;
}
