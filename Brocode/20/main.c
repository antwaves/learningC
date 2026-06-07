#include <stdio.h>

// otherwise written as
// typedef enum {
//     SUNDAY = 1, MONDAY = 2, TUESDAY = 3, WEDNESDAY = 4, 
//     THURSDAY = 5, FRIDAY = 6, SATURDAY = 7
// }Day;

enum Day {  // automatically goes up by one starting at 0, but can also set as equals to
    SUNDAY = 1, MONDAY = 2, TUESDAY = 3, WEDNESDAY = 4, 
    THURSDAY = 5, FRIDAY = 6, SATURDAY = 7
};

int main() {
    enum Day today = SUNDAY;

    if (today == SUNDAY || today == SATURDAY) {
        printf("Weekend time");
    } else {
        printf("Not weeend time");
    }
    return 0;
}

