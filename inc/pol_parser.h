#ifndef _PARSER_H
#define _PARSER_H

#include "outils.h"
#include "pol.h"
#include "sym_table.h"
#include "tree.h"

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
    SyntaxError
};

struct pol_parser_ctx_t {
    u8 nvars;
    char ** var_lst;  // ordered list of vars
    u64  *  var_cntr; // var counters
    enum parser_ctx_status status;
};


void ee_free(ee_t *);
void ee_print(ee_t * ee);
int  str_varlist_lookup(char * , char **, int );
void generate_term(void * , COEFTYPE, pp_ctx_t *);
rbtree_t * create_set_for_expr_l();
void destroy_set_for_expr_l(set_t *);
void set_insert(rbtree_t * rbt, ee_t * d, u8 n);

void print_var(sym_table_t *, char *);
ee_t * resolve_var_as_expression(sym_table_t * , char * );
ee_t * resolve_number_as_expression(sym_table_t * , float );
ee_t * resolve_op_expression(sym_table_t *, ee_t *, ee_t *, char *);
void   change_pol_order(char *);

#endif