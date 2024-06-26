#include "pol_parser.h"


void ee_free(ee_t * ee) {
    if (ee) {
        if (ee->v) {
            if (strcmp(ee->t, "aapol") == 0) aapol_free(ee->v);
            else if (strcmp(ee->t, "llpol") == 0) llpol_free(ee->v);
            else free(ee->v);
        }
        
        if (ee->t) free(ee->t);
        free(ee);
    }
}


int str_varlist_lookup(char * str, char **var_lst, int n) {
    for (int i = 0; i < n; i++) {
        if (strcmp(str, var_lst[i]) == 0) return i;
    }

    return -1;
}


void generate_term(void * aux_pol, coef_t coef, i32 p, pp_ctx_t * ctx) {
    //u64 exp = s_exp_pack(ctx->var_cntr, ctx->nvars);
    u64 * exp = calloc(sizeof(u64), ctx->nvars);
    
    for (int i = 0; i < ctx->nvars; i++) exp[i] = ctx->var_cntr[i];

    llpol_addterm(aux_pol, coef, exp, p, ctx->order);

    for (int i = 0; i < ctx->nvars; i++) ctx->var_cntr[i] = 0;

    free(exp);
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


void print_status(enum parser_ctx_status status) {
    if (status == Ok)           printf("Ok");
    if (status == SyntaxError)  printf("SyntaxError");
    if (status == NameError)    printf("NameError");
    if (status == TypeError)    printf("TypeError");

}


ee_t * resolve_var_as_expression(sym_table_t * st, char * var, i32 p, pp_ctx_t * ctx) {
    ee_t * ee = NULL;
    ste_t * entry = st_probe(st, var);
    if (!entry) {
        fprintf(stderr, "undefined variable: %s\n", var); 
        return NULL;
    } else {
        ee = malloc(sizeof(*ee));
        ee->t = strdup(entry->t);
        
        if (strcmp(ee->t, "llpol") == 0) {
            ee->v = llpol_create(ctx->nvars);
            llpol_cpy(ee->v, entry->v, ctx->order);
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

ee_t * get_object_from_var(sym_table_t * st, char * var) {
    ee_t * ee = NULL;
    ste_t * entry = st_probe(st, var);
    if (!entry) {
        fprintf(stderr, "undefined variable: %s\n", var); 
        return NULL;
    } else {
        ee = malloc(sizeof(*ee));
        ee->t = strdup(entry->t);
        
        if (strcmp(ee->t, "llpol") == 0) {
            ee->v = (llpol_t *)entry->v;
        } else if (strcmp(ee->t, "aapol") == 0) {
            ee->v = (aapol_t *)entry->v;
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

ee_t * resolve_op_expression(sym_table_t * st, ee_t * e1, ee_t * e2, char * op, i32 p, pp_ctx_t * ctx) {
    //printf("heeeeeee!\n");
    //printf("e1: %f - e2: %f\n", *(float *)e1->v, *(float *)e2->v);
    ee_t * ee = NULL;

    if (e1 != NULL && e2 != NULL) {
        if (strcmp(e1->t, "number") == 0) {
            if (strcmp(e2->t, "number") == 0) {
                ee = malloc(sizeof(*ee)); CHECKPTR(ee);
                ee->t = strdup("number");
                ee->v = malloc(sizeof(*ee->v)); CHECKPTR(ee->v);
                //printf("e1 OP e2: %f\n", *(float *)e1->v + *(float *)e2->v);
                
                if (strcmp(op, "+") == 0)
                    *(coef_t *)ee->v = *(coef_t *)e1->v + *(coef_t *)e2->v;
                
                if (strcmp(op, "-") == 0)
                    *(coef_t *)ee->v = *(coef_t *)e1->v - *(coef_t *)e2->v;
                    
                if (strcmp(op, "*") == 0)
                    *(coef_t *)ee->v = *(coef_t *)e1->v * *(coef_t *)e2->v;
                
                if (strcmp(op, "/") == 0)
                    *(coef_t *)ee->v = *(coef_t *)e1->v / *(coef_t *)e2->v;
            }

            if (strcmp(e2->t, "aapol") == 0) {
                ee = malloc(sizeof(*ee));
                ee->t = strdup("aapol");

                if (strcmp(op, "+") == 0) {
                    ee->v = aapol_create(ctx->nvars);
                    aapol_cpy(ee->v, e2->v);
                    aapol_addterm(ee->v, *(coef_t *)e1->v, 0);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = aapol_create(ctx->nvars);
                    aapol_cpy(ee->v, e2->v);
                    aapol_addterm(ee->v, -1 * *(coef_t *)e1->v, 0);
                }
                
                if (strcmp(op, "*") == 0)
                    ee->v = aapol_coef_multiply(e2->v, *(coef_t *)e1->v);

                if (strcmp(op, "/") == 0) {
                    fprintf(stderr, "TypeError: cannot divide by pol object");
                    ee_free(ee);
                    return NULL;
                }
            }

            if (strcmp(e2->t, "llpol") == 0) {
                ee = malloc(sizeof(*ee));
                ee->t = strdup("llpol");

                if (strcmp(op, "+") == 0) {
                    ee->v = llpol_create(ctx->nvars);
                    llpol_cpy(ee->v, e2->v, ctx->order);
                    llpol_addterm(ee->v, *(coef_t *)e1->v, 0, p, ctx->order);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = llpol_create(ctx->nvars);
                    llpol_cpy(ee->v, e2->v, ctx->order);
                    llpol_addterm(ee->v, -1 * *(coef_t *)e1->v, 0, p, ctx->order);
                }
                
                if (strcmp(op, "*") == 0)
                    ee->v = llpol_coef_multiply(e2->v, *(coef_t *)e1->v, p);

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
                    ee->v = aapol_create(ctx->nvars);
                    aapol_cpy(ee->v, e1->v);
                    //printf("copy done!: "); aapol_print(ee->v);
                    aapol_addterm(ee->v, *(coef_t *)e2->v, 0);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = aapol_create(ctx->nvars);
                    aapol_cpy(ee->v, e1->v);
                    aapol_addterm(ee->v, -1 * *(coef_t *)e2->v, 0);
                }
                
                if (strcmp(op, "*") == 0)
                    ee->v = aapol_coef_multiply(e1->v, *(coef_t *)e2->v);

                if (strcmp(op, "/") == 0) {
                    ee->v = aapol_coef_multiply(e1->v, 1 / *(coef_t *)e2->v);
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
                    fprintf(stderr, "TypeError: cannot divide by pol object");
                    ee_free(ee);
                    return NULL;
                }
            }

            if (strcmp(e2->t, "llpol") == 0) {
                fprintf(stderr, "TypeError: cannot operate diff poltypes object");
                ee_free(ee);
                ctx->status = TypeError;
                return NULL;
            }

        }

        if (strcmp(e1->t, "llpol") == 0) {
            ee = malloc(sizeof(*ee)); CHECKPTR(ee);
            ee->t = strdup("llpol");

            if (strcmp(e2->t, "number") == 0) {
                if (strcmp(op, "+") == 0) {
                    ee->v = llpol_create(ctx->nvars);
                    llpol_cpy(ee->v, e1->v, ctx->order);
                    //printf("copy done!: "); llpol_print(ee->v);
                    llpol_addterm(ee->v, *(coef_t *)e2->v, 0, p, ctx->order);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = llpol_create(((aapol_t *)e1->v)->nvar);
                    llpol_cpy(ee->v, e1->v, ctx->order);
                    llpol_addterm(ee->v, -1 * *(coef_t *)e2->v, 0, ctx->order, p);
                }
                
                if (strcmp(op, "*") == 0)
                    ee->v = llpol_coef_multiply(e1->v, *(coef_t *)e2->v, p);

                if (strcmp(op, "/") == 0) {
                    ee->v = llpol_coef_multiply(e1->v, 1 / *(coef_t *)e2->v, p);
                }
            }

            if (strcmp(e2->t, "llpol") == 0) {
                if (strcmp(op, "+") == 0) {
                    ee->v = llpol_add(e1->v, 1, e2->v, 1, p, ctx->order);
                }

                if (strcmp(op, "-") == 0) {
                    ee->v = llpol_add(e1->v, 1, e2->v, -1, p, ctx->order);
                }

                if (strcmp(op, "*") == 0) {
                    //ee->v = llpol_multiply(e1->v, e2->v);
                    fprintf(stderr, "error: multiplication is not implemented yet");
                    ctx->status = TypeError;
                    return NULL;
                }

                if (strcmp(op, "/") == 0) {
                    fprintf(stderr, "error: cannot divide by pol object");
                    ee_free(ee);
                    ctx->status = TypeError;
                    return NULL;
                }
            }

            if (strcmp(e2->t, "aapol") == 0) {
                fprintf(stderr, "TypeError: cannot operate diff poltypes object");
                ee_free(ee);
                ctx->status = TypeError;
                return NULL;
            }

        }
    }

    return ee;
}


void ee_print(ee_t * ee) {
    if (strcmp("number", ee->t) == 0) printf("%f", *(float *)ee->v);
    if (strcmp("llpol", ee->t) == 0) llpol_print(ee->v);
    if (strcmp("aapol", ee->t) == 0) aapol_print(ee->v);
}


int aapol_monomial_cmp_wrap(const void * a, const void * b, void * param) {
    int cmp = aapol_monomial_cmp((aapol_t *)a, (aapol_t *)b, *(enum MONOMIAL_ORDER*)param);

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

void set_insert(rbtree_t * rbt, ee_t * d, i32 p, pp_ctx_t * ctx) {
    if(rbt == NULL) SAYNEXITWERROR("set is null");
    if(d == NULL) SAYNEXITWERROR("d is null");
    llpol_t * pol;

    if (strcmp(d->t, "number") == 0) {
        pol = llpol_create(ctx->nvars);
        llpol_addterm(pol, *(float *)d->v, 0, p, ctx->order);
    }

    if (strcmp(d->t, "llpol") == 0) {
        pol = (llpol_t *)d->v;
    }

    rbtree_probe(rbt, pol);

    // todo: llpol
}


void f4_wrapper(rbtree_t * in, rbtree_t * out, i32 p, pp_ctx_t * ctx) {
    tictac_t tictac;
    struct timespec ts0, ts1;
    settictac2zero(&tictac);
    print_tictac(&tictac);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts0);
    f4_v1(in, out, p, ctx->order, &tictac);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);
    TIMESPEC_DIFF(ts0, ts1, ts0);
    TIMESPEC_ADD(tictac.total, tictac.total,ts0);
    print_tictac(&tictac);
    printf("Done!\n");
}


void llpol_pretty_print(llpol_t * llpol, char ** vars) {
    //printf("vars: '%s' n: %d", vars, llpol->n);
    //if (strlen(vars) - 1 != llpol->n) SAYNEXITWERROR("Check your vars");

     if (llpol == NULL) SAYNEXITWERROR("llpol is null");
    lpol_t * p = llpol->first;

    while (p) {
        if (p->coef >= 0) printf("+");
        
        if (d_exp_is_zero(p->exp, llpol->n) == 0) {
            printf("%d", p->coef);
            p = p->nxt;
            continue;
        }

        int i = 0;

        if (p->coef != 1) printf("%d", p->coef);
        else {
            i = 0;
            while (*(p->exp + i) == 0) {i++;}
            if (*(p->exp + i) == 1) printf("%s", vars[i++]);
            else printf("%s^%ld", vars[i++],*(p->exp + i));

        }

        for (/* empty */; i < llpol->n; i++) {
            if (*(p->exp + i) != 0)
                if (*(p->exp + i) == 1) printf("*%s", vars[i]);
                else printf("*%s^%ld", vars[i],*(p->exp + i));
        }
        p = p->nxt;
    }

}


void set_print(rbtree_t * rbt, pp_ctx_t * ctx) {
    rbt_trav_t trav;
    llpol_t * pol;

    for (pol = rbtree_trav_first(&trav, rbt); pol != NULL; pol = rbtree_trav_next(&trav)) {
        llpol_pretty_print(pol, ctx->var_lst);
        printf("\n");
    }
}


void varset_print(rbtree_t * rbt) {
    rbt_trav_t trav;
    char * pol;

    for (pol = rbtree_trav_first(&trav, rbt); pol != NULL; pol = rbtree_trav_next(&trav)) {
        printf("'%s' ", pol);
    }
    printf("\n");
}


void change_mon_order(pp_ctx_t * ctx, char * order) {
    if (strcmp("lex", order) == 0) ctx->order = lex;
    else if (strcmp("glex", order) == 0) ctx->order = glex;
    else if (strcmp("revlex", order) == 0) ctx->order = revlex;
    else if (strcmp("grevlex", order) == 0) ctx->order = grevlex;
    else printf("not implemented: %s order\n", order);
}


void update_vars(pp_ctx_t * ctx, rbtree_t * _vars) {
    rbt_trav_t t;
    char * v;
    int i;
    for (i = 0; i < ctx->nvars; i++) free(ctx->var_lst[i]);
    free(ctx->var_lst);
    ctx->nvars = _vars->sz;
    ctx->var_lst = malloc((ctx->nvars + 1) * sizeof(*ctx->var_lst));
    i = 0;

    for (v = rbtree_trav_first(&t, _vars); v != NULL; v = rbtree_trav_next(&t)) {
        ctx->var_lst[i++] = v;
    }
    
    ctx->var_lst[i] = NULL;
    rbtree_empty(_vars, NULL);
}

char * get_mon_order_str(pp_ctx_t* pctx) {
    if (pctx->order == lex) return strdup("lex");
    if (pctx->order == glex) return strdup("glex");
    if (pctx->order == grevlex) return strdup("grevlex");
    if (pctx->order == revlex) return strdup("revlex");
}