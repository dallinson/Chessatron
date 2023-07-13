#include "utils.hpp"

#include <cstdio>
#include <immintrin.h>

#include "magic_numbers.hpp"

void print_bitboard(uint64_t to_print) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            if (GET_BIT(to_print, (rank * 8) + file)) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

int bitboard_to_idx(uint64_t bitboard) {
    return _tzcnt_u64(bitboard);
};

uint64_t idx_to_bitboard(int idx) {
    return BIT(idx);
};

int pop_min_bit(uint64_t *num) {
    int to_return = bitboard_to_idx(*num);
    CLEAR_BIT(*num, to_return);
    return to_return;
}