#include <wchar.h>
#include <stdbool.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>


wchar_t get_middle(bool left, bool right) {
    if (!left && !right) { return L'│'; }
    else if (!left && right) { return L'├'; }
    else if (left && !right) { return L'┤'; }
    else { return L'┼'; }
}


void print_tiling(void* bits, size_t size) {
    uint32_t board = *((uint32_t*)bits) << (32 - size);
    int power = 0;
    int s = size;
    while (s > 1) { s >>= 1; power++; }

    wchar_t** rows = malloc(sizeof(wchar_t*) * 3);
    uint8_t size_bits = power;
    uint8_t board_size = (board >> (32 - size_bits));
    int ending_chars = 2;

    board_size = (board_size < (size - size_bits)) ? board_size : size - size_bits;

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

    rows[0][pos] = L'\0';
    rows[1][pos] = L'\0';
    rows[2][pos] = L'\0';

    wprintf(L"%ls%ls%ls", rows[0], rows[1], rows[2]);

    for (int i = 0; i < 3; i++) {
        free(rows[i]);
    }
    free(rows);
}


uint32_t* get_tilings(int n) {
    if (n == 0) {
        uint32_t* val = malloc(sizeof(uint32_t));
        *val = 0;
        return val;
    }
    
    if (n == 1) {
        uint32_t* val = malloc(sizeof(uint32_t));
        // *val = ;
        return val;
    }
    return 0;
}


int main() {
    setlocale(LC_ALL, "en_US.UTF-8"); 
    uint8_t* board = malloc(sizeof(uint16_t));
    *board = 0b10100000;

    print_tiling(board, 8);

    free(board);
}

// use a bitmask to repersent the tiles