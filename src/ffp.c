#include "ffp.h"

i32 gcd(i32 a, i32 b, i32 * x, i32 * y) {
    x = 1, y = 0;
    i32 x1 = 0, y1 = 1, a1 = a, b1 = b;

    while (b1) {
        i32 q = a1 / b1;
        x  = x1;
        x1 = x - q * x1;
        y  = y1;
        y1 = y - q * y1;
        a1 = b1;
        b1 = a1 - q * b1;
    }

    return a1;
}

i32 modp_multiply(i32 p, i32 x, i32 y) {
    if (x == 1) return y;
    if (y == 1) return x;
    i64 p64 = (i64)p;
    i64 x64 = (i64)x;
    i64 y64 = (i64)y;
    i64 r;

    if (x == -1 || x == p - 1) 
        return (i32)((p64 - y64) % p64);
    if (y == -1 || y == p - 1) 
        return (i32)((p64 - x64) % p64);
    
    r = x64 * y64;
    r = r % p64;

    return (i32)r;
}


i32 modp_add(i32 p, i32 x, i32 y) {
    if (x == 0) return y;
    if (y == 0) return x;

    i64 p64 = (i64) p;

    i64 r = (i64)x + (i64)y;
    
    r = r % p64;

    if (r < 0) r = p64 + r;

    return (i32) r;
}


i32 modp_axpy(i32 p, i32 a, i32 x, i32 y) {
    i64 p64 = (i64)p;
    i64 a64 = (i64)a;
    i64 x64 = (i64)x;
    i64 y64 = (i64)y;
    i64 r;

    if (a == 0 || x == 0) return y;

    if (a == 1) {
        r = x64 + y64;
    } else if (a == p - 1 || a == -1) {
        r = y64 - x64;
    } else {
        r = (a64 * x64) % p64;
        r = r + y64;
    }

    r = r % p64;

    if (r < 0) r = r + p64;

    return (i32) r;

}
i32 modp_inv(i32 p, i32 a) {
    i32 r;
    i32 x, y;
    i32 d = gcd(a, p, &x, &y);

    r = d < 0 ? (-x) % p : x % p;

    return x < 0 ? x + p : x;
}