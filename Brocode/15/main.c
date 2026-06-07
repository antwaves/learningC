#include <stdio.h>

int main() {

    int numbers[] = {10, 20, 30, 40, 50, 40, 20, 20};
    char grades[] = {'A', 'B', 'C', 'D', 'F'};
    char name[] = "Annie";

    numbers[0] = 100;

    for (int i = 0; i < sizeof(numbers) / sizeof(int); i++) {
        printf("%d\n", numbers[i]);
    }

    int scores[5] = {0}; // implicitly sets zeros

    for (int i = 0; i < sizeof(scores) / sizeof(int); i++) {
        printf("Enter a score: ");
        scanf_s(" %d", &scores[i]);

    }

    for (int i = 0; i < sizeof(scores) / sizeof(int); i++) {
        printf("%d ", scores[i]);
    }  

    return 0;
}

