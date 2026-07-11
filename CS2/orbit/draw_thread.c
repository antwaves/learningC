#include <signal.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>
#include <stdatomic.h>
#include <math.h>

#include "app.h"
#include "draw_frame.c"
#include "timing.c"

static volatile sig_atomic_t not_interrupted = 1;
static void sig_handler(int _) { // catches and handles interrupts to allow for cleanup
    not_interrupted = 0;
}


DWORD WINAPI start_threaded_draw_loop(LPVOID still_running) {
    struct App* a = ((struct App*)still_running);
    struct timespec ts;
    uint64_t prev_time = get_nanosecond_time(&ts);
    long double frame_time = 1.0f / 60.0f;

    while (a->still_running && not_interrupted) {
        uint64_t start_time = get_nanosecond_time(&ts);
        double delta_time = (double)(start_time - prev_time) * 1e-6;

        if (a->before_draw != NULL) {
            a->before_draw(a, a->before_draw_arg);
        }
        _draw_frame(a);

        uint64_t end_time = get_nanosecond_time(&ts);
        long double elapsed_seconds = (long double)(end_time - start_time) * 1e-9;
        if (!atomic_load(&a->frame_buffer_resized)) {
            if (frame_time - elapsed_seconds > 0) {
                precise_sleep(frame_time - elapsed_seconds, &ts);
            }
        }
        prev_time = start_time;
    }
    return 0;
}  


void join_threaded_draw_call(HANDLE thread_handle) {
    WaitForSingleObject(thread_handle, INFINITE);
}
