#include <stdio.h>
#include "unittest_engine.h"
#include "pol.h"

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
    u64 * exp_x = unpackexp(x, n);

    for (int i = 0; i < n; i++) {
        assert(expected_exp_x[i] == exp_x[i], "unpackexp function is failing!");
    }

    u64 y = packexp(exp_x, 8);
    assert(x == y, "packexp function is failing!");

    FREE(exp_x);

    return 0;
}

static char * test_packexp() {
    u8 n = 8;
    u64 expected_x = 7164981176274674482;
    u64 exp_x[] = {0x63, 0x6f, 0x20, 0x6b, 0xa, 0x36, 0x63, 0x32};
    u64 x = packexp(exp_x, 8);
    assert(x == expected_x, "packexp function is failing!");

    return 0;
}

static char * test_packexp_with_zero() {
    u8 n = 8;
    u64 expected_x = 0;
    u64 exp_x[] = {0, 0, 0, 0, 0, 0, 0, 0};
    u64 x = packexp(exp_x, 8);
    assert(x == expected_x, "packexp function is failing!");

    return 0;
}


static char * test_aapol_addterm_equals() {
    u8 n = 1;
    aapol_t * aapol = aapol_malloc(n);
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

    aapol_t * expected_aapol = aapol_malloc(n);
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
    aapol_t * aapol = aapol_malloc(n);
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

    aapol_t * minor_aapol = aapol_malloc(n);
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
    llpol_t * llpol = llpol_malloc(n);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 1);
    llpol = llpol_addterm(llpol, 1, 2);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, -1, 7);

    llpol_t * expectec_llpol = llpol_malloc(n);
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
    llpol_t * llpol = llpol_malloc(n);
    llpol = llpol_addterm(llpol, 1, 4);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 1);
    llpol = llpol_addterm(llpol, 1, 2);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, -1, 7);

    llpol_t * other_llpol = llpol_malloc(n);
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
    llpol_t * llpol = llpol_malloc(n);
    llpol = llpol_addterm(llpol, 1, 4);
    llpol = llpol_addterm(llpol, 1, 5);
    llpol = llpol_addterm(llpol, 1, 1);
    llpol = llpol_addterm(llpol, 1, 2);
    llpol = llpol_addterm(llpol, 1, 3);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, 1, 7);
    llpol = llpol_addterm(llpol, -1, 7);

    llpol_t * other_llpol = llpol_malloc(n);
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
    llpol_t * llpol = llpol_malloc(n);
    llpol = llpol_addterm(llpol, 100, 4);
    llpol = llpol_addterm(llpol, 111, 5);
    llpol = llpol_addterm(llpol, 423, 1);
    llpol = llpol_addterm(llpol, 244, 2);
    llpol = llpol_addterm(llpol, 533, 3);
    llpol = llpol_addterm(llpol, 467, 7);
    llpol = llpol_addterm(llpol, 123, 7);

    llpol_t * cpyollpol = llpol_malloc(n);
    llpol_cpy(cpyollpol, llpol);

    assert(llpol_hard_cmp(cpyollpol, llpol) == 0, "original and copy must be equal!");

    llpol_free(llpol);
    llpol_free(cpyollpol);

    return 0;
}


static char * test_aapol_cpy() {
    u8 n = 6;
    aapol_t * aapol = aapol_malloc(n);
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

    aapol_t * cpyoaapol = aapol_malloc(aapol->nvar);
    aapol_cpy(cpyoaapol, aapol);

    assert(aapol_hard_cmp(cpyoaapol, aapol) == 0, "aapol_cpy function is failing!");

    aapol_free(aapol);
    aapol_free(cpyoaapol);
    return 0;
}


static char * test_llpol_inplace_coef_multiply() {
    u8 n = 7;
    llpol_t * llpol = llpol_malloc(n);
    llpol_addterm(llpol, 100, 4);
    llpol_addterm(llpol, 111, 5);
    llpol_addterm(llpol, 423, 1);
    llpol_addterm(llpol, 244, 2);
    llpol_addterm(llpol, 533, 3);
    llpol_addterm(llpol, 467, 7);
    llpol_addterm(llpol, 123, 7);

    llpol_t * expected_llpol = llpol_malloc(n);
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
    llpol_t * llpol = llpol_malloc(n);
    llpol_addterm(llpol, 100, 4);
    llpol_addterm(llpol, 111, 5);
    llpol_addterm(llpol, 423, 1);
    llpol_addterm(llpol, 244, 2);
    llpol_addterm(llpol, 533, 3);
    llpol_addterm(llpol, 467, 7);
    llpol_addterm(llpol, 123, 7);

    llpol_t * expected_llpol = llpol_malloc(n);
    llpol_addterm(expected_llpol, 23.5 * 100, 4);
    llpol_addterm(expected_llpol, 23.5 * 111, 5);
    llpol_addterm(expected_llpol, 23.5 * 423, 1);
    llpol_addterm(expected_llpol, 23.5 * 244, 2);
    llpol_addterm(expected_llpol, 23.5 * 533, 3);
    llpol_addterm(expected_llpol, 23.5 * 467, 7);
    llpol_addterm(expected_llpol, 23.5 * 123, 7);

    llpol_t * result = llpol_coef_multiply(llpol, 23.5);

    assert(llpol_hard_cmp(result, expected_llpol) == 0, "llpol_coef_multiply is failing!");

    llpol_free(expected_llpol);
    llpol_free(result);

    return 0;
}


static char * test_aapol_coef_multiply() {
    u8 n = 5;
    aapol_t * aapol = aapol_malloc(n);
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

    aapol_t * expected_aapol = aapol_malloc(n);
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
    aapol_t * aapol = aapol_malloc(n);
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

    aapol_t * expected_aapol = aapol_malloc(n);
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


static void all_tests() {
    run_uniitest(test_packexp);
    run_uniitest(test_unpackexp);
    run_uniitest(test_packexp_with_zero);
    run_uniitest(test_aapol_addterm_equals);
    run_uniitest(test_aapol_addterm_different_minor);
    run_uniitest(test_llpol_addterm_equals);
    run_uniitest(test_llpol_addterm_different_major);
    run_uniitest(test_llpol_addterm_different_minor);
    run_uniitest(test_aapol_cpy);
    run_uniitest(test_llpol_cpy);
    run_uniitest(test_llpol_inplace_coef_multiply);
    run_uniitest(test_llpol_coef_multiply);
    run_uniitest(test_aapol_coef_multiply);
    run_uniitest(test_aapol_inplace_coef_multiply);
}


int main(int argc, char * argv[]) {
    printf("Unit testing...\n\n");
    all_tests();
    printf("\nFailed %d from %d tests.\n", failed_tests, tests_run);

    return 0;
}