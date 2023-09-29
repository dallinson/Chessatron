#include "utils.hpp"

#include <cstdio>
#include <immintrin.h>

#include "magic_numbers.hpp"

void print_bitboard(Bitboard to_print) {
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
