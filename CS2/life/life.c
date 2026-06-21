#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

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

        b->cells[b->height] = malloc(line_length * sizeof(Cell));
        while (*line != '\0') {
            Cell c = {};
            c.active = *line != '.';
            c.neighbors = 0;

            Cell** cells = b->cells;
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


bool get_state(Board* b, int r, int c) {
    if (r < 0 || r >= b->height || c < 0 || c >= b->width){
        return false;
    }
    return b->cells[r][c].active;
}


void set_neighbors(Board* b, int r, int c) {
    Cell cell = b->cells[r][c];
    cell.neighbors = 0;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i || j ) {
                 cell.neighbors += get_state(b, r + i, c + j);
            }
        }   
    }
    b->cells[r][c] = cell;
}


void update_board(Board* b) {
    for (int i = 0; i < b->height; i++) {
        for (int j = 0; j < b->width; j++) {
            set_neighbors(b, i, j);
        }
    }

    for (int i = 0; i < b->height; i++) {
        for (int j = 0; j < b->width; j++) {
            update(&b->cells[i][j]);
        }
    }
}


void print_board(Board* b) {
    for (int i = 0; i < b->height; i++) {
        for (int j = 0; j < b->width; j++) {
            printf("%c", str_cell(&b->cells[i][j]));
        }
        printf("\n");
    }
}

int main() {
    Cell c = {false, 2}; 

    update(&c);

    FILE* file;
    errno_t err = fopen_s(&file, "thing.txt", "r"); 
    if (file == NULL) {
        printf("Failed to open file");
        return 1;
    }

    fseek (file, 0, SEEK_END);
    long length = ftell(file);
    fseek (file, 0, SEEK_SET);
    char* buffer = malloc(length + 1);
    if (buffer){
        fread(buffer, 1, length, file);
    }
    fclose(file);
    buffer[length + 1] = '\0';

    Board* b = init(buffer);
    
    for (int i = 0; i < 500; i++) {
        print_board(b);
        printf("Generation %d\n", i);
        Sleep(100);
        update_board(b);
        printf("\033[H");
    }
    del_board(b);
}