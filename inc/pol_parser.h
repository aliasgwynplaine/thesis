#ifndef _PARSER_H
#define _PARSER_H

#include "outils.h"
#include "pol.h"
#include "sym_table.h"

typedef struct polynomial_parser_ctx_t pol_parser_ctx_t;
typedef struct expr_entry_t ee_t;

struct expr_entry_t {
        char * t;
        void * v;
};

struct pol_parser_ctx_t {
    u8 nvar;
    char ** var_lst; // ordered list of vars
    int status;
};

enum parser_ctx_status {
    Ok,
    NameError,
    SyntaxError
};


void ee_free(ee_t *);
int str_varlist_lookup(char * , char **, int );
void generate_term(void * , COEFTYPE , u64 * , char ** , int );
void print_var(sym_table_t *, char *);
ee_t * resolve_var_as_expression(sym_table_t * , char * );
ee_t * resolve_number_as_expression(sym_table_t * , float );
ee_t * resolve_op_expression(sym_table_t *, ee_t *, ee_t *, char *);
void   print_expr(ee_t *);
void   change_pol_order(char *);

#endif