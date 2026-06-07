#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>

int main() {
    time_t rawtime = time(NULL);
    struct tm time_info;
    bool running = true;
    struct timespec ts;
    long double ms;
    long double ns;


    printf("Digital Clock\n");
    while (running) {
        rawtime = time(NULL);
        localtime_s(&time_info, &rawtime);

        char *merediem = time_info.tm_hour < 12 ? "AM" : "PM";
        int hours = time_info.tm_hour % 12;

        if (timespec_get(&ts, TIME_UTC) == TIME_UTC){
            ns = ts.tv_nsec;
        }
        ms = ns * 1e-6;


        printf("\r%02d:%02d:%02d:%03.0LF %s", hours, time_info.tm_min, time_info.tm_sec, ms, merediem);
        Sleep(1);

        // (*thing).whatever = thing->whatever
    }

    return 0;

    //6:44:04
}
