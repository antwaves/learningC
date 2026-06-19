#include "../../csv/csv_handle.c"

int main() {
    ht_hash_table** games = csv_to_dict_list("games.csv");

    char* eagles = "Philadelphia Eagles";
    int wins = 0;
    int losses = 0;
    int ties = 0;

    ht_hash_table** p = games;
    while (*p != NULL) {
        ht_hash_table* row = *p;
        int eagles_p = -1;
        int other_p = -1;

        if (strcmp(ht_search(row, "home_team"), eagles) == 0) {
            eagles_p = atoi(ht_search(row, "home_score"));
            other_p = atoi(ht_search(row, "away_score"));
        }
        else if (strcmp(ht_search(row, "away_team"), eagles) == 0) {
            eagles_p = atoi(ht_search(row, "away_score"));
            other_p = atoi(ht_search(row, "home_score"));
        }

        p++;
        if (eagles_p == -1) {
            continue;
        }

        if (eagles_p > other_p) {
            wins++;
        }
        else if (eagles_p < other_p) {
            losses++;
        }
        else {
            ties++;
        }

    }

    printf("Wins: %d\nLosses: %d\nTies: %d\n", wins, losses, ties);
}