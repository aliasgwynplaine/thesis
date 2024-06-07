#include <immintrin.h>
#include <stdio.h>
#include <memory.h>
#include <xmmintrin.h>

__m128i __attribute__((aligned(32))) p;
int p_int = 524287;

__m128i __attribute__((aligned(32))) x[20], y[20];

void int2m128i(int32_t i) {
    p = _mm_set1_epi32(i);
}


__m128i add_mod_31_epu32(__m128i x, __m128i y) {
    __m128i a = _mm_add_epi32(x, y);
    return _mm_min_epu32(a, _mm_sub_epi32(a, p));
}


void print128_num(__m128i var) {
    int32_t val[4];
    memcpy(val, &var, sizeof(val));
    printf("num: %d %d %d %d \n", 
           val[0], val[1], val[2], val[3]);
}


int main() {
    int2m128i((int32_t)p_int);

    for (int i = 0; i < 20; i++) {
        x[i] = _mm_setr_epi32(i+1, i*2+1, i*3+1,i*4+1);

        y[i][0] = 7*i + 1;
        y[i][1] = 9;
    }
    
    print128_num(p);

    for (int i = 0; i < 20; i++) {
        printf("x: ");
        print128_num(x[i]);
        printf("y: ");
        print128_num(y[i]);
        __m128i b = add_mod_31_epu32(x[i], y[i]);
        printf("sum: ");
        print128_num(b);
    }

}