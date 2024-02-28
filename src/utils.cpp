#include "utils.hpp"

#include <cstdio>

#include "magic_numbers.hpp"

void print_bb(Bitboard to_print) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            if (get_bit(to_print, (rank * 8) + file)) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

bool is_aligned(int sq_1, int sq_2, int sq_3) { return (MagicNumbers::AlignedSquares[(64 * sq_1) + sq_2] & idx_to_bb(sq_3)) != 0; }