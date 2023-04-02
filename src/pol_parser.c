#include "pol_parser.h"

int str_varlist_lookup(char * str, char **var_lst, int n) {
    for (int i = 0; i < n; i++) {
        if (strcmp(str, var_lst[i]) == 0) return i;
    }

    return -1;
}


void generate_term(llpol_t * aux_pol, COEFTYPE coef, u64 * var_cntr, 
                    char ** var_lst, int nvars) {
    u64 exp = exp_pack(var_cntr, nvars);
    llpol_addterm(aux_pol, coef, exp);

    for (int i = 0; i < nvars; i++) var_cntr[i]=0;
}

void print_var(sym_table_t * st, char * var) {
    ste_t * entry = st_probe(st, var);

    if (!entry) printf("undefined variable: %s\n", var);
    else {
        if (strcmp(entry->t, "aapol") == 0) aapol_print((aapol_t *)entry->v);
        if (strcmp(entry->t, "llpol") == 0) llpol_print((llpol_t *)entry->v);
        if (strcmp(entry->t, "number") == 0) printf("%f\n", *(float *)entry->v);
    } 
}

ee_t * resolve_var_as_expression(sym_table_t * st, char * var) {
    ee_t * ee = NULL;
    ste_t * entry = st_probe(st, var);
    if (!entry) {
        fprintf(stderr, "undefined variable: %s\n", var); return NULL;
    } else {
        ee = malloc(sizeof(*ee));
        ee->t = strdup(entry->t);
        
        if (strcmp(ee->t, "llpol") == 0) {
            ee->v = llpol_create(((llpol_t *)entry->v)->nvar);
            llpol_cpy(ee->v, entry->v);
        } else if (strcmp(ee->t, "aapol") == 0) {
            ee->v = aapol_create(((aapol_t *)entry->v)->nvar);
            aapol_cpy(ee->v, entry->v);
        } else {
            ee->v = malloc(sizeof(float));
            *(float *)ee->v = *(float *) entry->v;
        }
    }

    return ee;
}

ee_t * resolve_number_as_expression(sym_table_t * st, float f) {
    ee_t * ee = malloc(sizeof(*ee)); CHECKPTR(ee);
    ee->t = strdup("number");
    ee->v = malloc(sizeof(float)); CHECKPTR(ee->v);
    *(float *)ee->v = f;
    
    return ee;
}