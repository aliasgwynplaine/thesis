#ifndef _UNITTEST_ENGINE_H
#define _UNITTEST_ENGINE_H

#include <stdio.h>

#ifdef assert
#undef assert
#endif
#define assert(test, msg) do {                                           \
    if (!(test)) {                                                       \
        fprintf(stderr, "[!] %s %d: %s\n", __FUNCTION__, __LINE__, msg); \
        return 1;                                                        \
    }                                                                    \
} while (0)

#define run_uniitest(test) do {  \
    int t = test(); tests_run++; \
    if (t == 1) {                \
        failed_tests++;          \
    }                            \
} while(0)

extern int tests_run;
extern int failed_tests;

#endif