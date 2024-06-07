#ifndef _FF_P_H
#define _FF_P_H

// finite field operations

#include "outils.h"

typedef i32 coef_t;

i32 gcd(i32 a, i32 b, i32 * x, i32 * y);
i32 modp_multiply(i32 p, i32 x, i32 y);
i32 modp_add(i32 p, i32 x, i32 y);
i32 modp_axpy(i32 p, i32 a, i32 x, i32 y);
i32 modp_inv(i32 p, i32 x);


#endif