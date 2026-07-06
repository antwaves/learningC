#include <signal.h>
#include <windows.h>

#include "app.h"
#include "draw_frame.c"

static volatile sig_atomic_t keep_running = 1;
static void sig_handler(int _) {
    keep_running = 0;
}


DWORD WINAPI start_threaded_draw_loop(LPVOID still_running) {
    struct App* a = ((struct App*)still_running);
    while (a->still_running && keep_running) {
        if (a->before_draw != NULL) {
            a->before_draw(a);
        }
        _draw_frame(a);
    }
    return 0;
}


void join_threaded_draw_call(HANDLE thread_handle) {
    WaitForSingleObject(thread_handle, INFINITE);
}
