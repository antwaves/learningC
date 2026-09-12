#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <windows.h>
#include <stdbool.h>

// algorithim constants
#define n 624
#define m 397
#define w 32
#define r 31
#define UMASK (0xffffffffUL << r)
#define LMASK (0xffffffffUL >> (w-r))
#define u 11
#define s 7
#define t 15
#define l 18
#define a 0x9908b0dfUL
#define b 0x9d2c5680UL
#define c 0xefc60000UL
#define d UINT32_MAX


int randint(int min, int max);
uint32_t mersenne_twist(int seed);
void initalize(uint32_t* mt_state, int seed);
void twist(uint32_t* mt_state, uint32_t* twisted_mt_state);
uint32_t temper(uint32_t* mt_state);


int main() {
    bool still_playing = true;

    while (still_playing) {
        char choice = '\0';
        while (!(choice == 'r' || choice == 'p' || choice == 's')){
            printf("Choose between rock, paper, and scissors (r/p/s): ");
            scanf_s(" %c", &choice);
        }

        int computer_choice_int = randint(0, 2);
        char computer_choice = '\0';
        if (computer_choice_int == '0') {
            computer_choice = 'r';
        } 
        else if (computer_choice_int == '1') {
            computer_choice = 'p';
        } 
        else {
            computer_choice = 's';
        }

        printf("You chose %c, the computer chose %c\n", choice, computer_choice);

        if (choice == computer_choice) {
            printf("Tie!!!\n");
            continue;
        }

        if (choice == 'r') {
            if (computer_choice == 'p') {
                printf("Computer wins\n");
            }
            else {
                printf("Player wins\n");
            }

        }

        else if (choice == 'p') {
            if (computer_choice == 'r') {
                printf("Player wins\n");
            }
            else {
                printf("Computer wins\n");
            }
        }

        else {
            if (computer_choice == 'r') {
                printf("Computer wins\n");
            }
            else {
                printf("Player wins\n");
            }
        }

        printf("\n");
    }




    return 0;
}


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
    y = y ^ ((y << t) & c);
    y = y ^ (y >> l);
    y = y & d;

    return y;
}
