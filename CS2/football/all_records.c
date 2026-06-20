#include "../../csv/csv_handle.c"
#include <stdbool.h>
#include <stdio.h>

void increment_table_item(ht_hash_table* ht, char* item);
bool in(char** list, int size, char* item);
int search_avoid_null(ht_hash_table* ht, char* item);

int main() {
    ht_hash_table** games = csv_to_dict_list("games.csv");

    ht_hash_table* wins = ht_new();
    ht_hash_table* losses = ht_new();
    ht_hash_table* ties = ht_new();
    int size = 0;
    char** teams = malloc(sizeof(char*) * 120);

    ht_hash_table** p = games;
    while (*p != NULL) {
        ht_hash_table* row = *p;
        char* home_team = ht_search(row, "home_team");
        int home_score = atoi(ht_search(row, "home_score"));
        char* away_team = ht_search(row, "away_team");
        int away_score = atoi(ht_search(row, "away_score"));

        char* winning_team;
        char* losing_team;

        if (!in(teams, size, home_team)) {
            teams[size++] = _strdup(home_team);
        }

        if (!in(teams, size, away_team)) {
            teams[size++] = _strdup(away_team);
        }

        if (home_score == away_score) {
            increment_table_item(ties, home_team);
            increment_table_item(ties, away_team);
            p++;
            continue;
        }

        if (home_score < away_score) {
            winning_team = away_team;
            losing_team = home_team;
        }   
        else if (home_score > away_score) {
            winning_team = home_team;
            losing_team = away_team;
        }

        increment_table_item(wins, winning_team);
        increment_table_item(losses, losing_team);
        p++;
    }
    teams[size] = NULL;

    for (int i = 0; i < size; i++) {
        char* team = teams[i];
        int win_amount = search_avoid_null(wins, team);
        int loss_amount = search_avoid_null(losses, team);
        int tie_amount = search_avoid_null(ties, team);

        float win_percentage = win_amount / (float)(win_amount + loss_amount + tie_amount);

        //int win_amount = atoi(wins) 
        printf("Team: %s\nWins: %d\nLosses: %d\n", team, win_amount, loss_amount);
        printf("Ties: %d\nWin Percentage: %.3f\n\n", tie_amount, win_percentage);
    }


    ht_del_hash_table(wins);
    ht_del_hash_table(losses);
    ht_del_hash_table(ties);
    free(teams);
}

bool in(char** list, int size, char* item){
    for (int i = 0; i < size; i++) {
        if (strcmp(list[i], item) == 0) {
            return true;
        }
    }
    
    return false;
}

void increment_table_item(ht_hash_table* ht, char* item) {
    char num_buffer[64];
    char* current = ht_search(ht, item);
    if (current != NULL){
        int c = atoi(current);
        c++;
        snprintf(num_buffer, 64, "%d", c);
        ht_insert(ht, item, num_buffer); 
    }
    else {
        ht_insert(ht, item, "1"); 
    } 
}


int search_avoid_null(ht_hash_table* ht, char* item) {
    char* current = ht_search(ht, item);
    if (current != NULL){
        return atoi(current);
    } 
    else {
        return 0;
    }
}
