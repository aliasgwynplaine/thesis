#include "pol.h"
#include <stdlib.h>
#include <inttypes.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/*
how do i control the vars???
x0,x1,x2
*/
struct pol_t {
    float coef;
    u64 exp;
    pol_t * nxt_pol;
};

void free_pol_t(pol_t * pol) {
    /*
    traverse the linked list and free.
    */
}

void print_pol(pol_t * pol) {

}