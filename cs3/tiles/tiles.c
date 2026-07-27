#include <wchar.h>
#include <stdbool.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>


wchar_t get_middle(bool left, bool right) {
    // 0 = 1, 1 = 2
    if (!left && !right) { return L'│'; }
    else if (!left && right) { return L'├'; }
    else if (left && !right) { return L'┤'; }
    else { return L'┼'; }
}


void print_tiling(uint32_t board) {
    wchar_t** rows = malloc(sizeof(wchar_t*) * 3);
    uint8_t size_bits = 5;
    uint8_t board_size = (board >> (32 - size_bits));
    int ending_chars = 2;

    int to_alloc = 1;
    for (int i = 0; i < board_size; i++) {
        bool tile = (board << (size_bits + i)) >> (31);
        to_alloc += !(board << (size_bits + i)) >> (31) ? 1 : 3;
        to_alloc += i < (board_size - 1);
    }
    to_alloc++;

    for (int i = 0; i < 3; i++) {
        rows[i] = malloc(sizeof(wchar_t) * (to_alloc + (ending_chars - (i == 2))));
    }

    int pos = 0;
    rows[0][pos] = L'┌';
    rows[1][pos] = get_middle(0, (board << size_bits) >> (31));
    rows[2][pos] = L'└';
    pos++;
    for (int i = 0; i < board_size; i++) {
        bool tile = (board << (size_bits + i)) >> (31);
        if (!tile) {
            rows[0][pos] = L'─';
            rows[1][pos] = L' ';
            rows[2][pos] = L'─';
            pos++;
        } else {
            for (int j = 0; j < 3; j++) {
                rows[0][pos] = L'─';
                rows[1][pos] = L'─';
                rows[2][pos] = L'─';
                pos++;
            }
        }

        if (i < board_size - 1) {
            rows[0][pos] = L'┬';
            rows[1][pos] = get_middle(tile, (board << (size_bits + i + 1)) >> (31));
            rows[2][pos] = L'┴';
            pos++;
        }
    }
    rows[0][pos] = L'┐';
    rows[1][pos] = get_middle((board << (size_bits + board_size - 1)) >> (31), 0);
    rows[2][pos] = L'┘';
    pos++;

    rows[0][pos] = L'\n';
    rows[1][pos] = L'\n';
    rows[2][pos] = L'\n';
    pos++;

    rows[0][pos++] = L'\0';
    rows[1][pos] = L'\0';
    rows[2][pos] = L'\0';

    wprintf(L"%ls%ls%ls", rows[0], rows[1], rows[2]);

    for (int i = 0; i < 3; i++) {
        free(rows[i]);
    }
    free(rows);
}


int main() {
    setlocale(LC_ALL, "en_US.UTF-8"); 
    uint32_t board = 0 | (0b00110011001 << 21);
    printf("%d\n", board);
    printf("%d\n", board >> 27);
    printf("%d\n", (board << 5) >> (31));

    print_tiling(board);

}

// use a bitmask to repersent the tiles