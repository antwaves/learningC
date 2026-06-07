#include <stdio.h>
#include <string.h>

int main() {
    int day = 0;
    char *day_name[10];

    printf("What is the day num: ");
    scanf_s("%d", &day);

    switch (day) {
        case 1:
            *day_name = "Monday";
            break;
        case 2:
            *day_name = "Tuesday";
            break;
        case 3:
            *day_name = "Wednesday";
            break;
        case 4:
            *day_name = "Thursday";
            break;
        case 5:
            *day_name = "Friday";
            break;
        case 6:
            *day_name = "Saturday";
            break;
        case 7:
            *day_name = "Sunday";
            break;
        default:
            *day_name = "";
            printf("fuck off");
            break;
    }

    if (strlen(*day_name)) {
        printf("It is %s girlie", *day_name);
    }
    
    //1:37:00
    return 0;
}
