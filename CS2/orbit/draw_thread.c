#include <signal.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>
#include <stdatomic.h>

#include "app.h"
#include "draw_frame.c"
#include "timing.c"

static volatile sig_atomic_t keep_running = 1;
static void sig_handler(int _) {
    keep_running = 0;
}


DWORD WINAPI start_threaded_draw_loop(LPVOID still_running) {
    struct App* a = ((struct App*)still_running);
    struct timespec ts;
    uint64_t prev_time = get_nanosecond_time(&ts);

    while (a->still_running && keep_running) {
        uint64_t start_time = get_nanosecond_time(&ts);
        double delta_time = (double)(start_time - prev_time) * 1e-6;

        printf("Took %.2f ms\n", delta_time);

        if (a->before_draw != NULL) {
            a->before_draw(a);
        }
        _draw_frame(a);

        uint64_t end_time = get_nanosecond_time(&ts);
        long double elapsed_seconds = (long double)(end_time - start_time) * 1e-9;
        if (!atomic_load(&a->frame_buffer_resized)) {
            precise_sleep(1.0 / 60.0 - elapsed_seconds, &ts);

        }
        prev_time = start_time;
    }
    return 0;
}  


void join_threaded_draw_call(HANDLE thread_handle) {
    WaitForSingleObject(thread_handle, INFINITE);
}
