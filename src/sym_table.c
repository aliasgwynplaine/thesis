#include "sym_table.h"


sym_table_t * st_create(u64 size) {
    sym_table_t * st = malloc(sizeof(*st));
    st->sz = size;
    st->e = malloc(st->sz * sizeof(ste_t));
    
    for (int i = 0; i < st->sz; i++) st->e[i].h = -1;
    
    st->c = 0;

    return st;
}


void st_destroy(sym_table_t * st) {
    for (int i = 0; i < 0; i++) {
        if (st->e[i].h != -1) {
            // st->e[i].freefux(st->e->v);
            if (strcmp("aapol", st->e[i].t) == 0) aapol_free(st->e[i].v);
            if (strcmp("llpol", st->e[i].t) == 0) llpol_free(st->e[i].v);
            FREE(st->e[i].t);
            FREE(st->e[i].n);
        }
    }

    FREE(st->e);
    FREE(st);
}



/** 
 * @brief inserts an entry into the symbol table st
 * @param st pointer to the symbol table
 * @param n name of the variable
 * @param v pointer to the object
 * @param t type of object (e.g. "aapol" or "llpol")
 * @return
**/ 
void * st_insert(sym_table_t * st, char * n, void * v, char * t/*, freefux_t * ff*/) {
    u64 h = compute_hash(n, st->sz);
    
    while (st->e[h].h != -1) {
        if (strcmp(n, st->e[h].n) == 0) break;
        else h = (h + 1) % st->sz;
    }

    st->e[h].h = h;
    st->e[h].v = v;
    st->e[h].n = n;
    st->e[h].t = t;
    //st->e[h].freefux = ff;
    st->c++;

    /* todo: expand symtable if it gets too big */

    return NULL;
}

ste_t * st_probe(sym_table_t * st, char * n) {
    u64 h = compute_hash(n, st->sz);

    while (st->e[h].h != -1) {
        if (strcmp(n, st->e[h].n) == 0)
            return &st->e[h]; // found
        else h = (h + 1) % st->sz;
    }

    return NULL; // not found
}


void * st_delete(sym_table_t * st, const char * n) {
    int j, i = compute_hash(n, st->sz);
    ste_t * entry;

    while(st->e[i].h != -1) {
        if (strcmp(n, st->e[i].n) == 0) break;
        else i = (i + 1) % st->sz;
    }

    if (st->e[i].h == -1) return NULL;
    st->e[i].h == -1;
    st->c--;

    for (j = i + 1; st->e[i].h != -1; j = (j + 1) % st->sz, st->c--) {
        *entry = st->e[j];
        st->e[j].h = -1;
        st_insert(st, entry->n, entry->v, entry->t/*, entry->freefux*/);
    }
}


u64 compute_hash(const char * s, u64 table_sz) { 
    u64 h = 0;

    for (/* */; *s != '\0'; s++) {
        h = (SYM_TABLE_P * h + *s) % table_sz;
    }

    return h;
}

int print_sym_table(sym_table_t * st) {
    int enough = 10;
    char * str;
    char buff[enough];
    printf("\n| %5s | %10s | %6s | %64s |\n", "hash", "name", "type", "repr");

    for (int i = 0; i < st->sz; i++) {
        if (st->e[i].h != -1) {
            if (strcmp(st->e[i].t, "aapol") == 0) str = aapol_repr(st->e[i].v);
            if (strcmp(st->e[i].t, "llpol") == 0) str = strdup("NOT IMPLEMENTED... yet!");
            if (strcmp(st->e[i].t, "int") == 0) {
                snprintf(buff, enough, "%d",  *((int*)st->e[i].v));
                str = strdup(buff);
            }
            if (strcmp(st->e[i].t, "float") == 0) {
                snprintf(buff, enough, "%f",  *((float*)st->e[i].v));
                str = strdup(buff);
            }
            printf("| %5ld | %10s | %6s | %64s |\n", st->e[i].h, st->e[i].n, st->e[i].t, str);
            FREE(str);
        }
    }

    printf("\n");
}