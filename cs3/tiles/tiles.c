#include <wchar.h>
#include <stdbool.h>
#include <locale.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>


typedef struct {
    uint8_t size;
    void* tilings;
} tiling_list;
static uint8_t one_tile[] = {0b00100000, 0};
static uint8_t zero_tiles[] = {0b00000000, 0};
const int MAX_BITS = 32;


wchar_t get_middle(bool left, bool right) {
    if (!left && !right) { return L'│'; }
    else if (!left && right) { return L'├'; }
    else if (left && !right) { return L'┤'; }
    else { return L'┼'; }
}


void print_tiling(void* bits, size_t size) {
    uint32_t board = *((uint32_t*)bits) << (MAX_BITS - size);
    int power = 0;
    int s = size;
    while (s > 1) { s >>= 1; power++; }
    uint8_t size_bits = power;
    uint8_t board_size = (board >> (MAX_BITS - size_bits));
    int ending_chars = 2;
    board_size = (board_size < (size - size_bits)) ? board_size : size - size_bits;

    int to_alloc = 1;
    for (int i = 0; i < board_size; i++) {
        to_alloc += (!(board << (size_bits + i)) >> (MAX_BITS - 1)) + (((board << (size_bits + i)) >> (MAX_BITS - 1)) * 3); // add one for verticals, add three for horizontals
        to_alloc += i < (board_size - 1); 
    }
    to_alloc++;
    
    wchar_t** rows = malloc(sizeof(wchar_t*) * 3);
    for (int i = 0; i < 3; i++) {
        rows[i] = malloc(sizeof(wchar_t) * (to_alloc + (ending_chars - (i == 2))));
    }

    int pos = 0;
    rows[0][pos] = L'┌';
    rows[1][pos] = get_middle(0, (board << size_bits) >> (MAX_BITS - 1));
    rows[2][pos] = L'└';
    pos++;
    for (int i = 0; i < board_size; i++) {
        bool tile = (board << (size_bits + i)) >> (MAX_BITS - 1);
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


int fib(unsigned int n) {
    int a = 1;
    int b = 1; 
    int t;
    for (int i = 0; i < n - 1; ++i) {
        t = a + b; 
        a = b;
        b = t;
    }
    return b;
}


tiling_list get_tilings(unsigned int n) {
    if (n == 0) {
        tiling_list t = {8, zero_tiles};
        return t;
    }

    if (n == 1) {
        tiling_list t = {8, one_tile};
        return t;
    }

    int num_tilings = fib(n);
    uint64_t size = 1;
    int temp = n;
    while (temp > 1) {
        temp /= 2; 
        size *= 2; 
    }
    size += n;

    // bit hack to align to powers of two
    int i = 1;
    size--;
    while (i < 64) {
        size |= size >> i;
        i *= 2;
    }
    size++;
    size = size < 8 ? 8 : size;
    size = size > 64 ? 64 : size;  
    
    int size_bits = 0;
    int s = size;
    while (s > 1) { s >>= 1; size_bits++; }

    size_t bytes_per_tiling = (size + 7) / 8;
    void* tilings = calloc(num_tilings, bytes_per_tiling + 1);
    tiling_list tiles = {size, tilings};

    tiling_list other_tilings = get_tilings(n - 1); 
    int other_num_tilings = fib(n - 1);
    uint32_t first = ((uint32_t*)other_tilings.tilings)[0];
    int other_tile_size = other_tilings.size; 
    int other_size_bits = 0; // num of bits taken up by size section
    int o_s = size;
    while (o_s > 1) { o_s >>= 1; other_size_bits++; }

    for (int i = 0; i < other_num_tilings; i++) {
        void* other_tile_p = other_tilings.tilings + (i * other_tile_size * sizeof(char));
        uint32_t tiling = (*((uint32_t*)other_tile_p)) << (MAX_BITS - other_tile_size);
        int other_tile_size_bits = tiling >> (MAX_BITS - other_size_bits); // the actual number of bits to be read

        uint32_t other_board = tiling << other_size_bits; 
        other_board >>= other_size_bits + 1; // dont need to manually flip the new bit, since it's just a zero
        other_board |= ((other_tile_size_bits + 1) << (MAX_BITS - other_size_bits));

        uint8_t *dest = (uint8_t *)tilings + i * bytes_per_tiling;
        memcpy(dest, ((uint8_t *)&other_board) + 4 - bytes_per_tiling, bytes_per_tiling);
        printf("%d\n", other_board);
    }    
    printf("%d\n", *(uint8_t*)(tilings));

    if (other_tilings.tilings != one_tile && other_tilings.tilings != zero_tiles) {
        free(other_tilings.tilings);
    }

    return tiles;
}


int main() {
    setlocale(LC_ALL, "en_US.UTF-8"); 
    uint8_t board =  0b00100000;
    print_tiling(&board, 8);
    tiling_list t = get_tilings(5);
    free(t.tilings);
}
