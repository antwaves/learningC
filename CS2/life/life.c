#include <stdbool.h>
#include <stdio.h>

#include "../../string/split.c"

typedef struct {
    bool active;
    char neighbors;
} Cell;


void update(Cell* c) {
    bool a = c->active;
    char n = c->neighbors;
    c->active = n == 3 || (a && n == 2);
}


char str_cell(Cell* c) {
    return c->active ? '#' : '.';
}


typedef struct {
    Cell** cells;
    int height;
    int width;
} Board;


Board* init(char* board_str) {
    Board* b = calloc(1, sizeof(Board));

    int n_lines = 1;
    char* c = board_str;
    while (*c != '\0') {
        if (*c == '\n') {
            n_lines++;
        }
        c++;
    }

    char** lines = split(board_str, '\n');
    if (lines == NULL) {
        return NULL;
    }


    b->cells = calloc(1, sizeof(Cell*) * n_lines);
    b->height = 0;

    int line_length = strlen(lines[0]);
    b->width = line_length;
    char** p = lines;
    while (*p != NULL) {
        char* line = *p;
        int index = 0;

        while (*line != '\0') {
            Cell c;
            c.active = *line != '.';
            c.neighbors = 0;

            Cell** cells = b->cells;
            cells[b->height] = malloc(strlen(lines[0]) * sizeof(Cell));

            cells[b->height][index++] = c;
            line++;

        }

        b->height++;
        p++;
    }

    


    free(lines);
    return b;
}


void del_board(Board* b) {
    for (int i = 0; i < b->height; i++) {
        free(b->cells[i]);
    }
    free(b->cells);
    free(b);
}


int main() {
    Cell c = {false, 2}; 

    update(&c);

    Board* b = init("..........\n..........\n..######..\n..........\n..........");
    del_board(b);
}