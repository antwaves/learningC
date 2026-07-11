#include <stdint.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#include "timing.h"
#pragma comment(lib, "winmm.lib")


uint64_t get_nanosecond_time(struct timespec* ts) {
    static LARGE_INTEGER frequency;
    static int initialized = 0;
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = 1;
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (uint64_t)(counter.QuadPart * 1000000000ULL / frequency.QuadPart);
}


void precise_sleep(long double seconds, struct timespec* ts) {
    static long double estimate = 5e-2;
    static long double mean = 5e-3;
    static long double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        uint64_t start = get_nanosecond_time(ts);
        Sleep(1);
        uint64_t end = get_nanosecond_time(ts);

        double observed = (double)(end - start) * 1e-9;
        seconds -= observed;

        ++count;
        long double delta = observed - mean;
        mean += delta / count;
        m2 += delta * (observed - mean);
        long double stddev = sqrt(m2 / count);
        estimate = mean + stddev;
    }

    uint64_t start = get_nanosecond_time(ts);
    while ((uint64_t)(get_nanosecond_time(ts) - start) * 1e-9 < seconds){ 
        SwitchToThread(); 
    };
}


void setup_precise_sleep() {
    timeBeginPeriod(1);
}


void close_precise_sleep() {
    timeEndPeriod(1);
}
