#include <stdio.h>
#include <math.h>
#include "unittest_engine.h"
#include "pol.h"
#include "matrix.h"
#include "tree.h"
#include "sym_table.h"

int tests_run    = 0;
int failed_tests = 0;

static char * test_example() {
    assert(1 != 1, "Missing tests!");
    return 0;
}


static char * test_unpackexp() {
    u8 n = 8;
    u64 x = 7164981176274674482;
    u64 expected_exp_x[] = {0x63, 0x6f, 0x20, 0x6b, 0xa, 0x36, 0x63, 0x32};
    u64 * exp_x = exp_unpack(x, n);

    for (int i = 0; i < n; i++) {
        assert(expected_exp_x[i] == exp_x[i], "exp_unpack function is failing!");
    }

    u64 y = exp_pack(exp_x, 8);
    assert(x == y, "exp_pack function is failing!");

    FREE(exp_x);

    return 0;
}


static char * test_packexp() {
    u8 n = 8;
    u64 expected_x = 7164981176274674482;
    u64 exp_x[] = {0x63, 0x6f, 0x20, 0x6b, 0xa, 0x36, 0x63, 0x32};
    u64 x = exp_pack(exp_x, n);
    assert(x == expected_x, "exp_pack function is failing!");

    return 0;
}


static char * test_packexp_with_zero() {
    u8 n = 8;
    u64 expected_x = 0;
    u64 exp_x[] = {0, 0, 0, 0, 0, 0, 0, 0};
    u64 x = exp_pack(exp_x, n);
    assert(x == expected_x, "exp_pack function is failing!");

    return 0;
}


static char * test_aapol_addterm_equals() {
    u8 n = 1;
    aapol_t * aapol = aapol_create(n);
    aapol_addterm(aapol, 1, 3);
    aapol_addterm(aapol, 1, 3);
    aapol_addterm(aapol, -1, 1);
    aapol_addterm(aapol, 1, 2);
    aapol_addterm(aapol, 1, 3);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 2);
    aapol_addterm(aapol, 1, 1);
    aapol_addterm(aapol, -1, 7);
    aapol_addterm(aapol, -1, 7);
    aapol_addterm(aapol, 7, 0);

    aapol_t * expected_aapol = aapol_create(n);
    aapol_addterm(expected_aapol, 3, 3);
    aapol_addterm(expected_aapol, 2, 2);
    aapol_addterm(expected_aapol, 2, 7);
    aapol_addterm(expected_aapol, 7, 0);

    assert(aapol_monomial_cmp(aapol, expected_aapol) == 0, "aapol_addterm function is failing!");

    aapol_free(aapol);
    aapol_free(expected_aapol);

    return 0;
}


static char * test_aapol_addterm_different_minor() {
    u8 n = 1;
    aapol_t * aapol = aapol_create(n);
    aapol_addterm(aapol, 1, 3);
    aapol_addterm(aapol, 1, 3);
    aapol_addterm(aapol, -1, 1);
    aapol_addterm(aapol, 1, 2);
    aapol_addterm(aapol, 1, 3);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 7);
    aapol_addterm(aapol, 1, 2);
    aapol_addterm(aapol, 1, 1);
    aapol_addterm(aapol, -1, 7);
    aapol_addterm(aapol, -1, 7);
    aapol_addterm(aapol, 7, 0);

    aapol_t * minor_aapol = aapol_create(n);
    aapol_addterm(minor_aapol, 3, 3);
    aapol_addterm(minor_aapol, 2, 1);
    aapol_addterm(minor_aapol, 2, 7);
    aapol_addterm(minor_aapol, 7, 0);

    assert(aapol_monomial_cmp(aapol, minor_aapol) > 0, "aapol_addterm function is failing!");

    aapol_free(aapol);
    aapol_free(minor_aapol);

    return 0;
}


static char * test_llpol_addterm_equals() {
    u8 n = 4;
    llpol_t * llpol = llpol_create(n);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 1);
    llpol = llpol_addterm(llpol, 1, 2);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, -1, 7);

    llpol_t * expectec_llpol = llpol_create(n);
    llpol_addterm(expectec_llpol, 3, 3);
    llpol_addterm(expectec_llpol, 1, 7);
    llpol_addterm(expectec_llpol, 1, 2);
    llpol_addterm(expectec_llpol, 1, 1);

    assert(llpol_hard_cmp(llpol, expectec_llpol) == 0, "llpol_addterm function is failing!");

    llpol_free(llpol);
    llpol_free(expectec_llpol);
    return 0;
}


static char * test_llpol_addterm_different_major() {
    u8 n = 4;
    llpol_t * llpol = llpol_create(n);
    llpol = llpol_addterm(llpol, 1, 4);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 1);
    llpol = llpol_addterm(llpol, 1, 2);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, -1, 7);

    llpol_t * other_llpol = llpol_create(n);
    llpol_addterm(other_llpol, 3, 3);
    llpol_addterm(other_llpol, 1, 7);
    llpol_addterm(other_llpol, 1, 2);
    llpol_addterm(other_llpol, 1, 1);

    assert(llpol_monomial_cmp(llpol, other_llpol) > 0, "llpol_addterm function is failing!");

    llpol_free(llpol);
    llpol_free(other_llpol);
    return 0;
}


static char * test_llpol_addterm_different_minor() {
    u8 n = 4;
    llpol_t * llpol = llpol_create(n);
    llpol = llpol_addterm(llpol, 1, 4);
    llpol = llpol_addterm(llpol, 1, 5);
    llpol = llpol_addterm(llpol, 1, 1);
    llpol = llpol_addterm(llpol, 1, 2);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, -1, 7);

    llpol_t * other_llpol = llpol_create(n);
    llpol_addterm(other_llpol, 3, 10);
    llpol_addterm(other_llpol, 1, 7);
    llpol_addterm(other_llpol, 1, 2);
    llpol_addterm(other_llpol, 1, 1);

    assert(llpol_monomial_cmp(llpol, other_llpol) < 0, "llpol_addterm function is failing!");

    llpol_free(llpol);
    llpol_free(other_llpol);
    return 0;
}


static char * test_llpol_cpy() {
    u8 n = 4;
    llpol_t * llpol = llpol_create(n);
    llpol = llpol_addterm(llpol, 100, 4);
    llpol = llpol_addterm(llpol, 111, 5);
    llpol = llpol_addterm(llpol, 423, 1);
    llpol = llpol_addterm(llpol, 244, 2);
    llpol = llpol_addterm(llpol, 533, 3);
    llpol = llpol_addterm(llpol, 467, 7);
    llpol = llpol_addterm(llpol, 123, 7);

    llpol_t * cpyollpol = llpol_create(n);
    llpol_cpy(cpyollpol, llpol);

    assert(llpol_hard_cmp(cpyollpol, llpol) == 0, "original and copy must be equal!");

    llpol_free(llpol);
    llpol_free(cpyollpol);

    return 0;
}


static char * test_aapol_cpy() {
    u8 n = 6;
    aapol_t * aapol = aapol_create(n);
    aapol_addterm(aapol, 11, 3525);
    aapol_addterm(aapol, 14, 6343);
    aapol_addterm(aapol, 16, 1245);
    aapol_addterm(aapol, 17, 2725);
    aapol_addterm(aapol, 31, 3967);
    aapol_addterm(aapol, 57, 7234);
    aapol_addterm(aapol, 81, 1457);
    aapol_addterm(aapol, 65, 5173);
    aapol_addterm(aapol, 13, 4247);
    aapol_addterm(aapol, 76, 6722);
    aapol_addterm(aapol, 34, 1624);
    aapol_addterm(aapol, -5, 7524);
    aapol_addterm(aapol, -1, 6724);
    aapol_addterm(aapol, -7, 2406);

    aapol_t * cpyoaapol = aapol_create(aapol->nvar);
    aapol_cpy(cpyoaapol, aapol);

    assert(aapol_hard_cmp(cpyoaapol, aapol) == 0, "aapol_cpy function is failing!");

    aapol_free(aapol);
    aapol_free(cpyoaapol);
    return 0;
}


static char * test_llpol_inplace_coef_multiply() {
    u8 n = 7;
    llpol_t * llpol = llpol_create(n);
    llpol_addterm(llpol, 100, 4);
    llpol_addterm(llpol, 111, 5);
    llpol_addterm(llpol, 423, 1);
    llpol_addterm(llpol, 244, 2);
    llpol_addterm(llpol, 533, 3);
    llpol_addterm(llpol, 467, 7);
    llpol_addterm(llpol, 123, 7);

    llpol_t * expected_llpol = llpol_create(n);
    llpol_addterm(expected_llpol, 53.5 * 100, 4);
    llpol_addterm(expected_llpol, 53.5 * 111, 5);
    llpol_addterm(expected_llpol, 53.5 * 423, 1);
    llpol_addterm(expected_llpol, 53.5 * 244, 2);
    llpol_addterm(expected_llpol, 53.5 * 533, 3);
    llpol_addterm(expected_llpol, 53.5 * 467, 7);
    llpol_addterm(expected_llpol, 53.5 * 123, 7);

    llpol_inplace_coef_multiply(llpol, 53.5);
    assert(llpol_hard_cmp(llpol, expected_llpol) == 0, "llpol and expected pol must be equal!");

    llpol_inplace_coef_multiply(llpol, 0.5);
    assert(llpol_hard_cmp(llpol, expected_llpol) != 0, "llpol and expected pol must be different!");

    llpol_free(llpol);
    llpol_free(expected_llpol);

    return 0;
}


static char * test_llpol_coef_multiply() {
    u8 n = 7;
    llpol_t * llpol = llpol_create(n);
    llpol_addterm(llpol, 100, 4);
    llpol_addterm(llpol, 111, 5);
    llpol_addterm(llpol, 423, 1);
    llpol_addterm(llpol, 244, 2);
    llpol_addterm(llpol, 533, 3);
    llpol_addterm(llpol, 467, 7);
    llpol_addterm(llpol, 123, 7);

    llpol_t * expected_llpol = llpol_create(n);
    llpol_addterm(expected_llpol, 23.5 * 100, 4);
    llpol_addterm(expected_llpol, 23.5 * 111, 5);
    llpol_addterm(expected_llpol, 23.5 * 423, 1);
    llpol_addterm(expected_llpol, 23.5 * 244, 2);
    llpol_addterm(expected_llpol, 23.5 * 533, 3);
    llpol_addterm(expected_llpol, 23.5 * 467, 7);
    llpol_addterm(expected_llpol, 23.5 * 123, 7);

    llpol_t * result = llpol_coef_multiply(llpol, 23.5);

    assert(llpol_hard_cmp(result, expected_llpol) == 0, "llpol_coef_multiply is failing!");

    llpol_free(llpol);
    llpol_free(expected_llpol);
    llpol_free(result);

    return 0;
}


static char * test_aapol_coef_multiply() {
    u8 n = 5;
    aapol_t * aapol = aapol_create(n);
    aapol_addterm(aapol, 11, 31564275);
    aapol_addterm(aapol, 14, 61364473);
    aapol_addterm(aapol, 16, 11264475);
    aapol_addterm(aapol, 17, 21764275);
    aapol_addterm(aapol, 31, 31964677);
    aapol_addterm(aapol, 57, 71264374);
    aapol_addterm(aapol, 81, 11464577);
    aapol_addterm(aapol, 65, 51164773);
    aapol_addterm(aapol, 13, 41264477);
    aapol_addterm(aapol, 76, 61764272);
    aapol_addterm(aapol, 34, 11664274);
    aapol_addterm(aapol, -5, 71564274);
    aapol_addterm(aapol, -1, 61764274);
    aapol_addterm(aapol, -7, 21464076);

    aapol_t * result = aapol_coef_multiply(aapol, 78.5);

    aapol_t * expected_aapol = aapol_create(n);
    aapol_addterm(expected_aapol, 78.5 * 11, 31564275);
    aapol_addterm(expected_aapol, 78.5 * 14, 61364473);
    aapol_addterm(expected_aapol, 78.5 * 16, 11264475);
    aapol_addterm(expected_aapol, 78.5 * 17, 21764275);
    aapol_addterm(expected_aapol, 78.5 * 31, 31964677);
    aapol_addterm(expected_aapol, 78.5 * 57, 71264374);
    aapol_addterm(expected_aapol, 78.5 * 81, 11464577);
    aapol_addterm(expected_aapol, 78.5 * 65, 51164773);
    aapol_addterm(expected_aapol, 78.5 * 13, 41264477);
    aapol_addterm(expected_aapol, 78.5 * 76, 61764272);
    aapol_addterm(expected_aapol, 78.5 * 34, 11664274);
    aapol_addterm(expected_aapol, 78.5 * -5, 71564274);
    aapol_addterm(expected_aapol, 78.5 * -1, 61764274);
    aapol_addterm(expected_aapol, 78.5 * -7, 21464076);

    assert(aapol_hard_cmp(result, expected_aapol) == 0, "result and expected aapol are not equal.");
    
    aapol_free(aapol);
    aapol_free(result);
    aapol_free(expected_aapol);

    return 0;
}


static char * test_aapol_inplace_coef_multiply() {
    u8 n = 5;
    aapol_t * aapol = aapol_create(n);
    aapol_addterm(aapol, 1816, 31564275);
    aapol_addterm(aapol, 1846, 61364473);
    aapol_addterm(aapol, 1866, 11264475);
    aapol_addterm(aapol, 1876, 21764275);
    aapol_addterm(aapol, 3816, 31964677);
    aapol_addterm(aapol, 5876, 71264374);
    aapol_addterm(aapol, 8816, 11464577);
    aapol_addterm(aapol, 6856, 51164773);
    aapol_addterm(aapol, 1836, 41264477);
    aapol_addterm(aapol, 7866, 61764272);
    aapol_addterm(aapol, 3846, 11664274);
    aapol_addterm(aapol, -856, 71564274);
    aapol_addterm(aapol, -816, 61764274);
    aapol_addterm(aapol, -876, 21464076);

    aapol_inplace_coef_multiply(aapol, 18.5);

    aapol_t * expected_aapol = aapol_create(n);
    aapol_addterm(expected_aapol, 18.5 * 1816, 31564275);
    aapol_addterm(expected_aapol, 18.5 * 1846, 61364473);
    aapol_addterm(expected_aapol, 18.5 * 1866, 11264475);
    aapol_addterm(expected_aapol, 18.5 * 1876, 21764275);
    aapol_addterm(expected_aapol, 18.5 * 3816, 31964677);
    aapol_addterm(expected_aapol, 18.5 * 5876, 71264374);
    aapol_addterm(expected_aapol, 18.5 * 8816, 11464577);
    aapol_addterm(expected_aapol, 18.5 * 6856, 51164773);
    aapol_addterm(expected_aapol, 18.5 * 1836, 41264477);
    aapol_addterm(expected_aapol, 18.5 * 7866, 61764272);
    aapol_addterm(expected_aapol, 18.5 * 3846, 11664274);
    aapol_addterm(expected_aapol, 18.5 * -856, 71564274);
    aapol_addterm(expected_aapol, 18.5 * -816, 61764274);
    aapol_addterm(expected_aapol, 18.5 * -876, 21464076);

    assert(aapol_hard_cmp(aapol, expected_aapol) == 0, "result and expected aapol are not equal.");
    
    aapol_free(aapol);
    aapol_free(expected_aapol);
    return 0;
}


static char * test_list_o_aapol2smatrix_transformation() { //todo
    //bstree_t * s = bstree_create(aapol_monomial_cmp, aapol_create, aapol_free);
    
    assert(0, "this test is missing!");

    return 0;
}


static char * test_aapol_add() {
    int n = 8;
    aapol_t * a = aapol_create(n);
    aapol_addterm(a, 1,                  0);
    aapol_addterm(a, 1,  72340172838076673);
    aapol_addterm(a, 1, 217020518514230019);
    aapol_addterm(a, 1, 144680345676153346);

    aapol_t * b = aapol_create(n);
    aapol_addterm(b, 1,                  0);
    aapol_addterm(b, 1,  72340172838076673);
    aapol_addterm(b, 1, 217020518514230019);
    aapol_addterm(b, 1, 144680345676153346);

    aapol_t * expected_result = aapol_create(n);
    aapol_addterm(expected_result, 2,                  0);
    aapol_addterm(expected_result, 2,  72340172838076673);
    aapol_addterm(expected_result, 2, 217020518514230019);
    aapol_addterm(expected_result, 2, 144680345676153346);

    aapol_t * result = aapol_add(a, 1, b, 1);

    assert(aapol_hard_cmp(result, expected_result) == 0, "aapols must be equals.");

    aapol_free(a);
    aapol_free(b);
    aapol_free(result);
    aapol_free(expected_result);

    a = aapol_create(n);
    
    aapol_addterm(a, 1,                  0);
    aapol_addterm(a, 1,  72340172838076673);
    aapol_addterm(a, 1, 217020518514230019);
    aapol_addterm(a, 1, 144680345676153346);
    aapol_addterm(a, 1,         8589934594);

    b = aapol_create(n);
    aapol_addterm(b, 1,                   0);
    aapol_addterm(b, 1,   72340172838076673);
    aapol_addterm(b, 1,  217020518514230019);
    aapol_addterm(b, 1,  144680345676153346);
    aapol_addterm(b, 1, 2305878202712596482);

    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 5,   72340172838076673);
    aapol_addterm(expected_result, 5,  217020518514230019);
    aapol_addterm(expected_result, 5,  144680345676153346);
    aapol_addterm(expected_result, 2,          8589934594);
    aapol_addterm(expected_result, 3, 2305878202712596482);
    aapol_addterm(expected_result, 5,                   0);

    result = aapol_add(a, 2, b, 3);

    assert(aapol_hard_cmp(result, expected_result) == 0, "aapols must be equals.");

    aapol_free(a);
    aapol_free(b);
    aapol_free(result);
    aapol_free(expected_result);

    a = aapol_create(n);
    
    aapol_addterm(a, 1,                  0);
    aapol_addterm(a, 1,  72340172838076673);
    aapol_addterm(a, 1, 217020518514230019);
    aapol_addterm(a, 1, 144680345676153346);
    aapol_addterm(a, 1,         8589934594);

    b = aapol_create(n);
    aapol_addterm(b, 1,                   0);
    aapol_addterm(b, 1,   72340172838076673);
    aapol_addterm(b, 1,  217020518514230019);
    aapol_addterm(b, 1,  144680345676153346);
    aapol_addterm(b, 1, 2305878202712596482);

    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 0,   72340172838076673);
    
    result = aapol_add(a, 0, b, 0);
    assert(aapol_hard_cmp(result, expected_result) == 0, "aapols must be equals and equal to zero");

    aapol_free(a);
    aapol_free(b);
    aapol_free(result);
    aapol_free(expected_result);

    return 0;
}


static char * test_int_max() {
    int a[] = {1,4,52,2,6,213,6,7,-123,213,5634,52,5,742,5,2,6,72345};
    int expectec_result = 72345;
    int result = int_max(a, 2, 17);

    assert(expectec_result == result, "int_max function is failing!");

    result = int_max(a, 0, 4);
    expectec_result = 52;

    assert(expectec_result == result, "int_max function is failing!");

    return 0;
}


static char * test_int_max_idx() {
    int a[] = {1,4,52,2,6,213,6,7,-123,213,5634,52,5,742,5,2,6,72345};
    int expectec_result = 17;
    int result = int_max_idx(a, 2, 17);

    assert(expectec_result == result, "int_max function is failing!");

    result = int_max_idx(a, 0, 4);
    expectec_result = 2;

    assert(expectec_result == result, "int_max function is failing!");

    return 0;
}


static char * test_aapol_multiply() {
    int n = 8;
    aapol_t * a = aapol_create(n);
    aapol_addterm(a, 1,                  0);
    aapol_addterm(a, 1,  72340172838076673); // (1,1,1,1,1,1,1,1)
    //aapol_addterm(a, 1, 217020518514230019); // (3,3,3,3,3,3,3,3)
    //aapol_addterm(a, 1, 144680345676153346); // (2,2,2,2,2,2,2,2)

    aapol_t * b = aapol_create(n);
    aapol_addterm(b, 1,                  0);
    aapol_addterm(b, 1,  72340172838076673); // (1,1,1,1,1,1,1,1)
    //aapol_addterm(b, 1, 217020518514230019); // (3,3,3,3,3,3,3,3)
    //aapol_addterm(b, 1, 144680345676153346); // (2,2,2,2,2,2,2,2)

    aapol_t * expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1,                  0);
    aapol_addterm(expected_result, 2,  72340172838076673);
    aapol_addterm(expected_result, 1, 144680345676153346);

    aapol_t * result = aapol_multiply(a, b);
    //printf("\nresult: "); aapol_print(result);
    //printf("expected_result: "); aapol_print(expected_result);

    assert(aapol_hard_cmp(result, expected_result) == 0, "aapols must be equals.");

    aapol_free(a);
    aapol_free(b);
    aapol_free(result);
    aapol_free(expected_result);

    a = aapol_create(n);
    aapol_addterm(a, 1, 72340172838076673);
    b = aapol_create(n);
    aapol_addterm(b, 1,                  0);
    aapol_addterm(b, 1,  72340172838076673);
    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1,  72340172838076673);
    aapol_addterm(expected_result, 1, 144680345676153346);

    result = aapol_multiply(a, b);

    assert(aapol_hard_cmp(result, expected_result) == 0, "aapols must be equals.");

    aapol_free(a);
    aapol_free(b);
    aapol_free(result);
    aapol_free(expected_result);

    return 0;
}


static char * test_exp_norm() {
    int n = 2;
    u64 e = 4294967297; // (1,1)
    assert(exp_norm(e, n) == sqrt(2), "exp norm is failing.");
    return 0;
}


static char * test_csr_load() {
    const char * fn = "mat_7x5_03.txt";
    FILE * fh = fopen(fn, "r");
    csr_t * csr = csr_load(fh);
    //csr_print(csr);
    csr_free(csr);
    fclose(fh);

    assert(1 == 2, "This test needs to be completed");

    return 0;
}


static char * test_aapol_list_sort() {
    assert(1!=1, "todo");
    return 0;
}


static char * test_llpol_list_sort() {
    assert(1!=1, "todo");
    return 0;
}


static char * test_str2llpol() {
    int n = 4;
    const char * var_lst[] = {"x", "y", "z", "w"};
    llpol_t * result = str2llpol("x^3 - 23 * x^2*y - 2 *x*y*z^3 + 5*x + 10 + w^5", var_lst, n);
    llpol_t * expected_result = llpol_create(n);
    llpol_addterm(expected_result, 1, 844424930131968);
    llpol_addterm(expected_result, -23, 562954248388608);
    llpol_addterm(expected_result, -2, 281479271874560);
    llpol_addterm(expected_result, 5, 281474976710656);
    llpol_addterm(expected_result, 1, 5);
    llpol_addterm(expected_result, 10, 0);
    assert(llpol_hard_cmp(result, expected_result) == 0, "str2llpol is not parsing well");

    llpol_free(result);
    llpol_free(expected_result);

    return 0;
}


static char * test_str2aapol() {
    int n = 4;
    const char * var_lst[] = {"x", "y", "z", "w"};
    aapol_t * result = str2aapol("x^3 - 23 * x^2*y - 2*x*y*z^3 + 5*x + 10 + w^5", var_lst, n);
    aapol_t * expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1, 844424930131968);
    aapol_addterm(expected_result, -23, 562954248388608);
    aapol_addterm(expected_result, -2, 281479271874560);
    aapol_addterm(expected_result, 5, 281474976710656);
    aapol_addterm(expected_result, 1, 5);
    aapol_addterm(expected_result, 10, 0);

    assert(aapol_hard_cmp(result, expected_result) == 0, "str2aapol is not parsing well");

    aapol_free(result);
    aapol_free(expected_result);

    result = str2aapol(" - 23 * x^2*y + x^3 - 2*x*y*z^3 + 5*x + 10 + w^5", var_lst, n);
    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1, 844424930131968);
    aapol_addterm(expected_result, -23, 562954248388608);
    aapol_addterm(expected_result, -2, 281479271874560);
    aapol_addterm(expected_result, 5, 281474976710656);
    aapol_addterm(expected_result, 1, 5);
    aapol_addterm(expected_result, 10, 0);

    assert(aapol_hard_cmp(result, expected_result) == 0, "str2aapol is not parsing well");

    aapol_free(result);
    aapol_free(expected_result);
    result = str2aapol("5.0 +5 - 23 * x^2*y + x^3 - 2*x*y*z^3 + 5*x + w^5 ", var_lst, n);
    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1, 844424930131968);
    aapol_addterm(expected_result, -23, 562954248388608);
    aapol_addterm(expected_result, -2, 281479271874560);
    aapol_addterm(expected_result, 5, 281474976710656);
    aapol_addterm(expected_result, 1, 5);
    aapol_addterm(expected_result, 10, 0);
    //printf("\n");
    //aapol_print(result);
    //aapol_print(expected_result);
    
    assert(aapol_hard_cmp(result, expected_result) == 0, "str2aapol is not parsing well");

    aapol_free(result);
    aapol_free(expected_result);
    result = str2aapol("+ w^5 +   5.0 +5 - 23 * x^2*y + x^3 - 2*x*y*z^3 + 5*x  ", var_lst, n);
    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1, 844424930131968);
    aapol_addterm(expected_result, -23, 562954248388608);
    aapol_addterm(expected_result, -2, 281479271874560);
    aapol_addterm(expected_result, 5, 281474976710656);
    aapol_addterm(expected_result, 1, 5);
    aapol_addterm(expected_result, 10, 0);
    //printf("\n");
    //aapol_print(result);
    //aapol_print(expected_result);
    
    assert(aapol_hard_cmp(result, expected_result) == 0, "str2aapol is not parsing well");

    aapol_free(result);
    aapol_free(expected_result);
    result = str2aapol("+    w^5 +   5.0 +5 - 23 * x^2 * y + x^3 - 2 *  x* y*  z^3 + 5*x  ", var_lst, n);
    expected_result = aapol_create(n);
    aapol_addterm(expected_result, 1, 844424930131968);
    aapol_addterm(expected_result, -23, 562954248388608);
    aapol_addterm(expected_result, -2, 281479271874560);
    aapol_addterm(expected_result, 5, 281474976710656);
    aapol_addterm(expected_result, 1, 5);
    aapol_addterm(expected_result, 10, 0);
    //printf("\n");
    //aapol_print(result);
    //aapol_print(expected_result);
    
    assert(aapol_hard_cmp(result, expected_result) == 0, "str2aapol is not parsing well");

    aapol_free(result);
    aapol_free(expected_result);

    return 0;
}


static char * test_str2aapol_name_error() {
    int n = 4;
    const char * var_lst[] = {"x", "y", "z", "w"};
    aapol_t * result = str2aapol("x^3 - 23 * x^2*y - 2*x*y*z^3*a + 5*x + 10 + w^5", var_lst, n);
    
    assert(result == NULL, "str2aapol is not parsing well");

    return 0;
}


static char * test_str2aapol_syntax_error() {
    int n = 4;
    const char * var_lst[] = {"x", "y", "z", "w"};
    aapol_t * result = str2aapol("x^3 - 23 *, x^2*y - 2*x1*y*z^3* + 5*x |+ 10 + _w^5", var_lst, n);
    
    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x^3 +----- 23 * x^2*y - 2*x1*y*z^3* + 5*x + 10 + w^5", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x^3 ++++ 23 * x^2*y - 2*x1*y*z^3* + 5*x + 10 + w^5", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x^^3 ++++ 23 * x^2*y - 2*x1*y*z^3* + 5*x + 10 + w^5", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x**3 + 23 * x^2*y - 2*x1*y*z^3* + 5*x + 10 + w^5", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x*3 + 23 * x^2*y - 2*x1*y*z^3* + 5*x + 10 + w^5", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x ^ 3 + 23 * x^2*y - 2*x1*y*z^3* + 5*x + 10 + w^5", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x^3 + 23 * x^2*y - 2*x*y*z^3ad + 5*x + 10 + w^5", var_lst, n);
    //aapol_print(result);

    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("xy^3", var_lst, n);

    assert(result == NULL, "str2aapol is not parsing well");
    result = str2aapol("41234- 3*x+ 1z523 + x*y^3", var_lst, n);
    
    assert(result == NULL, "str2aapol is not parsing well");

    result = str2aapol("x^z- 3*x+ 1z523 + x*y^3", var_lst, n);
    //printf("result: "); aapol_print(result);

    assert(result == NULL, "str2aapol is not parsing well");

    return 0;
}


static char * test_csr_head() {
    FILE * fh = fopen("mat_7x5_03.txt", "r");
    csr_t * csr = csr_load(fh);
    int i = csr_head_idx(csr, 2);
    assert(i == 2, "csr_head_idx is failling");

    i = csr_head_idx(csr, 3);
    assert(i == 1, "csr_head_idx is failling");

    i = csr_head_idx(csr, 5);
    assert(i == 0, "csr_head_idx is failling");

    fclose(fh);
    csr_free(csr);
    return 0;
}


static char * test_csr_analyse_n_decompose() {
    FILE * fh = fopen("mat80x100_05.txt", "r");
    csr_t * csr = csr_load(fh);
    dctx_t * piv = csr_analyse(csr);

    printf("-----------------\n");
    csr_dense_print(csr);
    printf("=========\n");
    tmat_t * mat = csr_decompose(csr);
    printf("A:\n");
    flsm_print(mat->a);
    printf("B:\n");
    flsm_print(mat->b);
    printf("C:\n");
    flsm_print(mat->c);
    printf("D:\n");
    flsm_print(mat->d);
    dctx_print(piv, csr->m, csr->n);
    printf("Done!");

    flsm_free(mat->a);
    flsm_free(mat->b);
    flsm_free(mat->c);
    flsm_free(mat->d);
    dctx_free(mat->dctx);
    FREE(mat);
    csr_free(csr);
    dctx_free(piv);
    fclose(fh);

    return 0;
}


static char * test_csr2flsm() {
    FILE * fh = fopen("mat80x100_05.txt", "r");
    csr_t * csr = csr_load(fh);
    flsm_t * flsm = csr2flsm(csr);
    flsm_print(flsm);
    dctx_t * dctx = flsm_analyse(flsm);
    dctx_print(dctx, flsm->m, flsm->n);;
    
    csr_free(csr);
    dctx_free(dctx);
    flsm_free(flsm);
    fclose(fh);

    return 0;
}


static char * test_sparse2dense() {
    idx_t n   = 4;
    idx_t dim = 10;
    COEFTYPE v[] = {1, 3, 6, 8};
    idx_t    i[] = {1, 3, 6, 8};
    COEFTYPE expected_result[] = {0, 1, 0, 3, 0, 0, 6, 0, 8, 0};
    COEFTYPE * dv = sparse2dense(v, i, n, dim);
    int cmp = 0;

    for (idx_t j = 0; j < dim; j++) {
        if (expected_result[j] != dv[j]) {
            cmp = 1;
            break;
        }
    }

    assert(cmp == 0, "sparse2dense is failing!");
    FREE(dv);

    return 0;
}

static char * test_aapol_repr() {
    int n = 4;
    const char * var_lst[] = {"x", "y", "z", "w"};
    aapol_t * result = str2aapol("x^3 - 23.0123 * x^2*y - 2.97641*x*y*z^3 + 5*x + 171030.8037 + w^5", var_lst, n);
    char * repr = aapol_repr(result);
    printf("aapol_repr: %s\n", repr);
    FREE(repr);
    aapol_free(result);

    return 0;
}


static char * test_sym_table_insert() {
    int n = 4;
    sym_table_t * st = st_create(127);
    const char * var_lst[] = {"x", "y", "z", "w"};
    aapol_t * a = str2aapol("x^3 - 23.01 * x^2*y - 2*x*y*z^3 + 5*x + 171.0 + w^5", var_lst, n);
    st_insert(st, "a", (void *)a, "aapol");
    print_sym_table(st);
    st_destroy(st);
    aapol_free(a);

    return 0;
}


static void all_tests() {
    run_unittest(test_packexp);
    run_unittest(test_unpackexp);
    run_unittest(test_packexp_with_zero);
    run_unittest(test_aapol_addterm_equals);
    run_unittest(test_aapol_addterm_different_minor);
    run_unittest(test_llpol_addterm_equals);
    run_unittest(test_llpol_addterm_different_major);
    run_unittest(test_llpol_addterm_different_minor);
    run_unittest(test_aapol_cpy);
    run_unittest(test_llpol_cpy);
    run_unittest(test_llpol_inplace_coef_multiply);
    run_unittest(test_llpol_coef_multiply);
    run_unittest(test_aapol_coef_multiply);
    run_unittest(test_aapol_inplace_coef_multiply);
    run_unittest(test_list_o_aapol2smatrix_transformation);
    run_unittest(test_aapol_add);
    run_unittest(test_int_max);
    run_unittest(test_int_max_idx);
    run_unittest(test_aapol_multiply);
    run_unittest(test_exp_norm);
    run_unittest(test_csr_load);
    run_unittest(test_str2llpol);
    run_unittest(test_str2aapol);
    run_unittest(test_str2aapol_name_error);
    run_unittest(test_str2aapol_syntax_error);
    run_unittest(test_csr_head);
    run_unittest(test_csr_analyse_n_decompose);
    //run_unittest(test_csr2flsm);
    run_unittest(test_sparse2dense);
    //run_unittest(cuda_test);
    run_unittest(test_aapol_repr);
    run_unittest(test_sym_table_insert);
}


int main(int argc, char * argv[]) {
    printf("Unit testing...\n\n");
    all_tests();
    printf("\nFailed %d from %d tests.\n", failed_tests, tests_run);

    return 0;
}