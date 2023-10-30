#include "pol_parser.h"


void ee_free(ee_t * ee) {
    if (ee) {
        if (ee->v) {
            if (strcmp(ee->t, "aapol") == 0) aapol_free(ee->v);
            else if (strcmp(ee->t, "llpol") == 0) btpol_free(ee->v);
            else FREE(ee->v);
        }
        
        if (ee->t) FREE(ee->t);
        FREE(ee);
    }
}


int str_varlist_lookup(char * str, char **var_lst, int n) {
    for (int i = 0; i < n; i++) {
        if (strcmp(str, var_lst[i]) == 0) return i;
    }

    return -1;
}


void generate_term(void * aux_pol, COEFTYPE coef, pp_ctx_t * ctx) {
    u64 exp = exp_pack(ctx->var_cntr, ctx->nvars);
    aapol_addterm(aux_pol, coef, exp);

    for (int i = 0; i < ctx->nvars; i++) ctx->var_cntr[i]=0;
}

void print_var(sym_table_t * st, char * var) {
    ste_t * entry = st_probe(st, var);

    if (!entry) printf("undefined variable: %s\n", var);
    else {
        if (strcmp(entry->t, "aapol") == 0) aapol_print((aapol_t *)entry->v);
        if (strcmp(entry->t, "llpol") == 0) btpol_print((btpol_t *)entry->v);
        if (strcmp(entry->t, "number") == 0) printf("%f\n", *(float *)entry->v);
    } 
}

ee_t * resolve_var_as_expression(sym_table_t * st, char * var) {
    ee_t * ee = NULL;
    ste_t * entry = st_probe(st, var);
    if (!entry) {
        fprintf(stderr, "undefined variable: %s\n", var); 
        return NULL;
    } else {
        ee = malloc(sizeof(*ee));
        ee->t = strdup(entry->t);
        
        if (strcmp(ee->t, "llpol") == 0) {
            ee->v = btpol_create(((aapol_t *)entry->v)->nvar);
            btpol_cpy(ee->v, entry->v);
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

ee_t * resolve_op_expression(sym_table_t * st, ee_t * e1, ee_t * e2, char * op) {
    //printf("heeeeeee!\n");
    //printf("e1: %f - e2: %f\n", *(float *)e1->v, *(float *)e2->v);
    ee_t * ee = NULL;

    if (e1 != NULL && e2 != NULL) {
        if (strcmp(e1->t, "number") == 0) {
            if (strcmp(e2->t, "number") == 0) {
                ee = malloc(sizeof(*ee)); CHECKPTR(ee);
                ee->t = strdup("number");
                ee->v = malloc(sizeof(float)); CHECKPTR(ee->v);
                //printf("e1 OP e2: %f\n", *(float *)e1->v + *(float *)e2->v);
                
                if (strcmp(op, "+") == 0)
                    *(float *)ee->v = *(float *)e1->v + *(float *)e2->v;
                
                if (strcmp(op, "-") == 0)
                    *(float *)ee->v = *(float *)e1->v - *(float *)e2->v;
                    
                if (strcmp(op, "*") == 0)
                    *(float *)ee->v = *(float *)e1->v * *(float *)e2->v;
                
                if (strcmp(op, "/") == 0)
                    *(float *)ee->v = *(float *)e1->v / *(float *)e2->v;
            }

            if (strcmp(e2->t, "aapol") == 0) {
                ee = malloc(sizeof(*ee));
                ee->t = strdup("aapol");

                if (strcmp(op, "+") == 0) {
                    ee->v = aapol_create(((aapol_t *)e2->v)->nvar);
                    aapol_cpy(ee->v, e2->v);
                    aapol_addterm(ee->v, *(float *)e1->v, 0);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = aapol_create(((aapol_t *)e2->v)->nvar);
                    aapol_cpy(ee->v, e2->v);
                    aapol_addterm(ee->v, -1 * *(float *)e1->v, 0);
                }
                
                if (strcmp(op, "*") == 0)
                    ee->v = aapol_coef_multiply(e2->v, *(float *)e1->v);

                if (strcmp(op, "/") == 0) {
                    fprintf(stderr, "error: cannot divide by pol object");
                    ee_free(ee);
                    return NULL;
                }
            }
        }

        if (strcmp(e1->t, "aapol") == 0) {
            ee = malloc(sizeof(*ee)); CHECKPTR(ee);
            ee->t = strdup("aapol");

            if (strcmp(e2->t, "number") == 0) {
                if (strcmp(op, "+") == 0) {
                    ee->v = aapol_create(((aapol_t *)e1->v)->nvar);
                    aapol_cpy(ee->v, e1->v);
                    //printf("copy done!: "); aapol_print(ee->v);
                    aapol_addterm(ee->v, *(float *)e2->v, 0);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = aapol_create(((aapol_t *)e1->v)->nvar);
                    aapol_cpy(ee->v, e1->v);
                    aapol_addterm(ee->v, -1 * *(float *)e2->v, 0);
                }
                
                if (strcmp(op, "*") == 0)
                    ee->v = aapol_coef_multiply(e1->v, *(float *)e2->v);

                if (strcmp(op, "/") == 0) {
                    ee->v = aapol_coef_multiply(e1->v, 1 / *(float *)e2->v);
                }
            }

            if (strcmp(e2->t, "aapol") == 0) {
                if (strcmp(op, "+") == 0) {
                    ee->v = aapol_add(e1->v, 1, e2->v, 1);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = aapol_add(e1->v, 1, e2->v, -1);
                }

                if (strcmp(op, "*") == 0) {
                    ee->v = aapol_multiply(e1->v, e2->v);
                }

                if (strcmp(op, "/") == 0) {
                    fprintf(stderr, "error: cannot divide by pol object");
                    ee_free(ee);
                    return NULL;
                }
            }
        }
    }

    return ee;
}

void ee_print(ee_t * ee) {
    if (strcmp("number", ee->t) == 0) printf("%f\n", *(float *)ee->v);
    if (strcmp("llpol", ee->t) == 0) btpol_print(ee->v);
    if (strcmp("aapol", ee->t) == 0) aapol_print(ee->v);
}

int aapol_monomial_cmp_wrap(const void * a, const void * b, void * param) {
    int cmp = aapol_monomial_cmp((aapol_t *)a, (aapol_t *)b);

    if (cmp == 0) {
        if (AAPOL_HEAD_COEF((aapol_t *)a) < AAPOL_HEAD_COEF((aapol_t *)b)) return -1;
        else if (AAPOL_HEAD_COEF((aapol_t *)a) > AAPOL_HEAD_COEF((aapol_t *)b)) return 1;
        else return 0;
    }
    else return cmp;
}

rbtree_t * create_set_for_expr_l() {
    rbtree_t * set = rbtree_create(aapol_monomial_cmp_wrap, NULL, NULL);

    return set;
}

void destroy_set_for_expr_l(set_t * s) {
    rbtree_destroy(s, NULL);
}

void set_insert(rbtree_t * rbt, ee_t * d, u8 n) {
    if(rbt == NULL) SAYNEXITWERROR("geee");
    if(d == NULL) SAYNEXITWERROR("gaa");
    aapol_t * pol;

    if (strcmp(d->t, "number") == 0) {
        pol = aapol_create(n);
        aapol_addterm(pol, *(float *)d->v, 0);
    }

    if (strcmp(d->t, "aapol") == 0) {
        pol = (aapol_t *)d->v;
    }

    rbtree_probe(rbt, pol);

    // todo: llpol
}

void f4_wrapper(rbtree_t * in, rbtree_t * out) {
    rbt_trav_t trav;
    aapol_t * pol;

    for (pol = rbtree_trav_first(&trav, in); pol != NULL; pol = rbtree_trav_next(&trav)) {
        aapol_t * newpol = aapol_add(pol, 1, pol, 1);
        rbtree_probe(out, newpol);
    }
}

void set_print(rbtree_t * rbt) {
    rbt_trav_t trav;
    aapol_t * pol;

    for (pol = rbtree_trav_first(&trav, rbt); pol != NULL; pol = rbtree_trav_next(&trav)) {
        aapol_print(pol);
    }
}