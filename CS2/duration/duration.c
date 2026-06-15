#include <math.h>
#include <stdio.h>

typedef struct {
    int hours;
    int minutes;
    int seconds;
    int total_seconds;
} Duration;


Duration init(int hours, int minutes, int seconds) {
    int total_seconds = hours * 3600 + minutes * 60 + seconds;
    
    int h = abs(total_seconds) / 3600;
    int m = ((abs(total_seconds) - (h * 3600))) / 60;
    int s = (abs(total_seconds) - (h * 3600) - (m * 60));

    Duration d = {h, m, s, total_seconds};
    return d;
}


char* to_str(Duration d, char* buffer) {
    snprintf(buffer, 128, "%s%02dh %02dm %02ds", d.total_seconds >= 0 ? "" : "-" , d.hours, d.minutes, d.seconds);
    return buffer;
}   

int main () {
    char buffer[128];
    printf("%s\n", to_str(init(1, 23, 4), buffer));
    printf("%s\n", to_str(init(0, 0, -100), buffer));
    printf("%s\n", to_str(init(-1, -1, -1), buffer));


    // i dont feel like doing the rest. its all just comparing the total_seconds
}