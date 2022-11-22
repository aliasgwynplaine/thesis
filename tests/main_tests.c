#include <stdio.h>
#include "unittest_engine.h"

int tests_run    = 0;
int failed_tests = 0;

static char * test_example() {
    assert(1 == 1, "Missing tests!");
    return 0;
}


static char * all_tests() {
    run_uniitest(test_example);

    return 0;
}


int main(int argc, char * argv[]) {
    printf("Running tests...\n");
    char * res = all_tests();
    printf("Done!\n");
    printf("Failed %d from %d tests.\n", failed_tests, tests_run);

    return 0;
}