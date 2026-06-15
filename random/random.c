#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "random.h"


int randint(int min, int max);
uint32_t mersenne_twist(int seed);
void initalize(uint32_t* mt_state, int seed);
void twist(uint32_t* mt_state, uint32_t* twisted_mt_state);
uint32_t temper(uint32_t* mt_state);


int randint(int min_val, int max_val) {
    struct timespec ts;
    uint64_t ns;
    if (timespec_get(&ts, TIME_UTC) == TIME_UTC){
        ns = ts.tv_nsec;
    }

    uint32_t seed = ns & 0xFFFFFFFF; // use an AND mask to get last 32 bits
    uint32_t m_twist = mersenne_twist(seed);
    float mult = (float)m_twist / (float)d;

    return (max_val  - min_val) * mult + min_val;
}


uint32_t mersenne_twist(int seed) {
    // performs a mersenne twist and returns a single value
    uint32_t mt_state[n];
    uint32_t twisted_mt_state[n];

    initalize(mt_state, seed);
    twist(mt_state, twisted_mt_state);
    uint32_t output = temper(twisted_mt_state);
    
    return output;
}


void initalize(uint32_t mt_state[], int seed) {
    uint32_t g = seed;
    for (int i = 1; i <= 51; i++) {
        g = 69069 * g + 1; // make the number large
    }

    for (int i = 0; i < n; i++) {
        g = 69069 * g + 1; // make it even larger
        mt_state[i] = g & 0xFFFFFFFF; // use an AND mask to get last 32 bits
    }
}


void twist(uint32_t* mt_state, uint32_t* twisted_mt_state) { // does some bullshit to create a "twisted" array
    for (int i = 0; i < n; i++) {
        uint32_t x = (mt_state[i] & UMASK) + (mt_state[(i + 1) % n] & LMASK);
        uint32_t xA = x >> 1;

        if (x % 2) {
            xA = xA ^ a;
        }
        twisted_mt_state[i] = mt_state[(i + m) % n] ^ xA;
    }
}


uint32_t temper(uint32_t* twisted_mt_state) { // evil bullshit
    uint32_t y = twisted_mt_state[0];

    y = y ^ ((y >> u) & d);
    y = y ^ ((y << s) & b);
    y = y ^ ((y << t) & _c);
    y = y ^ (y >> l);
    y = y & d;

    return y;
}
