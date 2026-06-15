#define n 624
#define m 397
#define _w 32
#define r 31
#define UMASK (0xffffffffUL << r)
#define LMASK (0xffffffffUL >> (_w-r))
#define u 11
#define s 7
#define t 15
#define l 18
#define a 0x9908b0dfUL
#define b 0x9d2c5680UL
#define _c 0xefc60000UL
#define d UINT32_MAX


int randint(int min_val, int max_val);