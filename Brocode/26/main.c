#include <stdio.h>
#include <stdlib.h>

int main() {
    int players = 0;
    printf("How many?: ");
    scanf_s(" %d", &players);

    int *scores = calloc(players, sizeof(int)); // safer, inits to 0

    if (scores == NULL) {
        printf("AAAAAAAAAa");
        return 1;
    }

    for (int i = 0; i < players; i++) {
        printf("Enter score #%d: ", i + 1);
        scanf_s(" %d", &scores[i]);
    }

    for (int i = 0; i < players; i++) {
        printf("%d ", scores[i]);
    }

    free(scores);
    scores = NULL;

    return 0;
}
