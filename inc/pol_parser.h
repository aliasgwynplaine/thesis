#ifndef _PARSER_H
#define _PARSER_H

#include "outils.h"
#include "pol.h"
#include "sym_table.h"
#include "tree.h"
#include "matrix.h"
#include "f4.h"

typedef struct pol_parser_ctx_t pp_ctx_t;
typedef struct expr_entry_t ee_t;
typedef rbtree_t set_t; // this can be changed.

struct expr_entry_t {
        char * t;
        void * v;
};

enum parser_ctx_status {
    Ok,
    NameError,
    SyntaxError,
    TypeError
};

struct pol_parser_ctx_t {
    u8 nvars;
    char ** var_lst;  // ordered list of vars
    u64  *  var_cntr; // var counters
    enum parser_ctx_status status;
    enum MONOMIAL_ORDER order;
};

enum pol_type {
    llpol_p,
    aapol_p
};

void ee_free(ee_t *);
void ee_print(ee_t * ee);
int  str_varlist_lookup(char * , char **, int );
void generate_term(void * , coef_t, i32 p, pp_ctx_t *);
rbtree_t * create_set_for_expr_l();
void destroy_set_for_expr_l(set_t *);
void set_insert(rbtree_t * rbt, ee_t * d, i32 p, pp_ctx_t *);
void f4_wrapper(rbtree_t * in, rbtree_t * out, i32 p, pp_ctx_t *);

void print_var(sym_table_t *, char *);
void print_status(enum parser_ctx_status);
ee_t * resolve_var_as_expression(sym_table_t * , char * , i32 p, pp_ctx_t *);
ee_t * get_object_from_var(sym_table_t * st, char * var);
ee_t * resolve_number_as_expression(sym_table_t * , float );
ee_t * resolve_op_expression(sym_table_t *, ee_t *, ee_t *, char *, i32, pp_ctx_t *);
void   change_mon_order(pp_ctx_t*, char *);
void   update_vars(pp_ctx_t *, char *);
char * get_mon_order_str(pp_ctx_t*);

#endif