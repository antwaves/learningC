#include <stdint.h>
#include <windows.h>
#include <time.h>
#include <math.h>


uint64_t get_nanosecond_time(struct timespec* ts) {
    timespec_get(ts, TIME_UTC);
    return ((uint64_t)ts->tv_sec * 1000000000ULL) + ts->tv_nsec;;
}


void precise_sleep(long double seconds, struct timespec* ts) {
    static HANDLE timer = NULL; 
    if (timer == NULL) {
        timer = CreateWaitableTimer(NULL, FALSE, NULL);
    }   
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds - estimate > 1e-7) {
        double to_wait = seconds - estimate;
        LARGE_INTEGER due;
        due.QuadPart = -(int64_t)(to_wait * 1e7);
        uint64_t start = get_nanosecond_time(ts);
        SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        uint64_t end = get_nanosecond_time(ts);

        double observed = (double)(end - start) * 1e-9;
        seconds -= observed;
        ++count;
        double error = observed - to_wait;
        double delta = error - mean;
        mean += delta / count;
        m2 += delta * (error - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    uint64_t start = get_nanosecond_time(ts);
    while ((double)(get_nanosecond_time(ts) - start) * 1e-9 < seconds){ YieldProcessor(); };
}