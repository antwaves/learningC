#include <stdint.h>
#include <time.h>

#ifndef TIMING
#define TIMING
uint64_t get_nanosecond_time(struct timespec* ts);
void precise_sleep(long double seconds, struct timespec* ts);
#endif
