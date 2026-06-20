#include "../../csv/csv_handle.c"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int wins; 
    int losses;
    int ties;
    int points_scored;
    int points;
    int points_allowed;
    int num_games;
} stats;


int find_in(char** list, int size, char* item);
int add_team_record(char** teams, int* size, stats** stats_list, char* team);

void update_team_stats(ht_hash_table* row, stats** stats_list, int h_index, int a_index);
void handle_base(stats* s, int own_score, int other_score);
void handle_tie(stats* s, int own_score, int other_score);

char* repr(stats* s, char* buffer, int size);
double win_percentage(stats* s);
double scored_per_game(stats* s);
double allowed_per_game(stats* s);


int main() {
    ht_hash_table** games = csv_to_dict_list("games.csv");
        
    int max_teams = 120;
    int size = 0;
    char** teams = calloc(max_teams, sizeof(char*));
    stats** stats_list = calloc(max_teams, sizeof(stats*));

    ht_hash_table** p = games;
    while (*p != NULL) {
        ht_hash_table* row = *p;
        char* home_team = ht_search(row, "home_team");
        char* away_team = ht_search(row, "away_team");


        int h_index =  add_team_record(teams, &size, stats_list, home_team);
        int a_index = add_team_record(teams, &size, stats_list, away_team);

        update_team_stats(row, stats_list, h_index, a_index);

        p++;
    }

    char buffer[2048];
    for (int i = 0; i < size; i++) {
        printf("%s\n", teams[i]);
        printf("%s", repr(stats_list[i], buffer, sizeof(buffer)));
    }

    free(teams);
    for (int i = 0; i <  size; i++) {
        free(stats_list[i]);
    }
    free(stats_list);
}


int add_team_record(char** teams, int* size, stats** stats_list, char* team) {
    int index = find_in(teams, *size, team);

    if (index == *size) {
        (*size)++;

        stats* s = calloc(1, sizeof(stats));
        if (stats_list[index] == NULL) {
            stats_list[index] = s;
        }
    }
    return index;
}


int find_in(char** list, int size, char* item){
    for (int i = 0; i < size; i++) {
        if (strcmp(list[i], item) == 0) {
            return i;
        }
    }
    
    list[size] = item;
    return size;
}


void update_team_stats(ht_hash_table* row, stats** stats_list, int h_index, int a_index) {
    int home_score = atoi(ht_search(row, "home_score"));
    int away_score = atoi(ht_search(row, "away_score"));


    int winning_score = 0;
    int losing_score = -0;
    stats* winning_team;
    stats* losing_team;

    if (home_score == away_score) {
        handle_tie(stats_list[h_index], home_score, away_score);
        handle_tie(stats_list[a_index], away_score, home_score);
        return;
    }


    if (home_score > away_score) {
        winning_team = stats_list[h_index];
        winning_score = home_score;
        losing_team = stats_list[a_index];
        losing_score = away_score;
    }
    else {
        winning_team = stats_list[a_index];
        winning_score = away_score;
        losing_team = stats_list[h_index];
        losing_score = home_score;
    }

    winning_team->wins += 1;
    losing_team->losses += 1;

    handle_base(winning_team, winning_score, losing_score);
    handle_base(losing_team, losing_score, winning_score);

}


void handle_tie(stats* s, int own_score, int other_score) {
    s->ties += 1;
    handle_base(s, own_score, other_score);
}


void handle_base(stats* s, int own_score, int other_score) {
    s->points_scored += own_score;
    s->points_allowed += other_score;
    s->num_games += 1;
}


char* repr(stats* s, char* buffer, int size) {

    snprintf(buffer, size, "number of games: %d\nwins: %d\nlosses: %d\nties: %d\nwin percent: %.3f\npoints scored: %d\npoints allowed: %d\npoints scored per game: %.3f\npoints allowed per game %.3f\n\n\n", s->num_games, s->wins, s->losses, s->ties, win_percentage(s), s->points_scored, s->points_allowed, scored_per_game(s), allowed_per_game(s));
    return buffer;
}


double win_percentage(stats* s) {
    return s-> wins / (double)s->num_games;
}


double scored_per_game(stats* s) {
    return s-> points_scored / (double)s->num_games;
}


double allowed_per_game(stats* s) {
    return s-> points_allowed / (double)s->num_games;
}
