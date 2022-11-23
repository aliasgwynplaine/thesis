#include <stdio.h>
#include "unittest_engine.h"
#include "pol.h"

int tests_run    = 0;
int failed_tests = 0;

static int test_example() {
    assert(1 != 1, "Missing tests!");
    return 0;
}


static int test_unpackexp() {
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

static int test_packexp() {
    u8 n = 8;
    u64 expected_x = 7164981176274674482;
    u64 exp_x[] = {0x63, 0x6f, 0x20, 0x6b, 0xa, 0x36, 0x63, 0x32};
    u64 x = packexp(exp_x, 8);
    assert(x == expected_x, "packexp function is failing!");

    return 0;
}

static int test_packexp_with_zero() {
    u8 n = 8;
    u64 expected_x = 0;
    u64 exp_x[] = {0, 0, 0, 0, 0, 0, 0, 0};
    u64 x = packexp(exp_x, 8);
    assert(x == expected_x, "packexp function is failing!");

    return 0;
}

static int test_aapol_addterm_equal() {
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

    assert(aapol_cmp(aapol, expected_aapol) == 0, "aapol_addterm function is failing!");

    aapol_free(aapol);
    aapol_free(expected_aapol);

    return 0;
}

static int test_aapol_addterm_different_minor() {
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

    assert(aapol_cmp(aapol, minor_aapol) > 0, "aapol_addterm function is failing!");

    aapol_free(aapol);
    aapol_free(minor_aapol);

    return 0;
}


static int test_llpol_addterm_equals() {
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

    assert(llpol_cmp(llpol, expectec_llpol) == 0, "llpol_addterm function is failing!");

    llpol_free(llpol);
    llpol_free(expectec_llpol);
    return 0;
}

static int test_llpol_addterm_different_major() {
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

    assert(llpol_cmp(llpol, other_llpol) > 0, "llpol_addterm function is failing!");

    llpol_free(llpol);
    llpol_free(other_llpol);
    return 0;
}


static int test_llpol_addterm_different_minor() {
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

    assert(llpol_cmp(llpol, other_llpol) < 0, "llpol_addterm function is failing!");

    llpol_free(llpol);
    llpol_free(other_llpol);
    return 0;
}


static void all_tests() {
    run_uniitest(test_packexp);
    run_uniitest(test_unpackexp);
    run_uniitest(test_packexp_with_zero);
    run_uniitest(test_aapol_addterm_equal);
    run_uniitest(test_aapol_addterm_different_minor);
    run_uniitest(test_llpol_addterm_equals);
    run_uniitest(test_llpol_addterm_different_major);
    run_uniitest(test_llpol_addterm_different_minor);
}


int main(int argc, char * argv[]) {
    printf("Running tests...\n\n");
    all_tests();
    printf("\nFailed %d from %d tests.\n", failed_tests, tests_run);

    return 0;
}