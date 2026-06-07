#include <stdio.h>
#include <stdbool.h>

int main() {
    int x = 5;
    int y = 6;
    int max = x > y ? x : y;
    printf("%d\n", max);

    bool online = true;
    printf("%s\n", online ? "online" : "offline");

    int num = 5;
    printf("%d is %s\n", num, (num % 2) ? "odd" : "even");

    int hours = 12;
    int mins = 3;
    char *merediem = hours < 12 ? "AM" : "PM";

    printf("%02d:%02d %s", hours, mins, merediem);

    return 0;
}

