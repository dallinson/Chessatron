#pragma once

#include <cstdint>
#include <immintrin.h>

#define BIT(x) (((uint64_t) 1) << (x))
#define GET_BIT(val, x) (((val) >> (x)) & 0x1)
#define SET_BIT(val, x) val |= BIT(x)
#define CLEAR_BIT(val, x) val &= ~BIT(x)
#define TOGGLE_BIT(val, x) val ^= BIT(x)

#define BITS(max, min) ((0xFFFFFFFFFFFFFFFF << (min)) & (0xFFFFFFFFFFFFFFFF >> (64 - ((max) + 1))))
#define GET_BITS(val, max, min) (((val) & BITS(max, min)) >> (min))
#define GET_RANK(val) GET_BITS(val, 5, 3)
#define GET_FILE(val) GET_BITS(val, 2, 0)

void print_bitboard(uint64_t to_print);
int bitboard_to_idx(uint64_t bitboard);
uint64_t idx_to_bitboard(int idx);

int pop_min_bit(uint64_t* num);

namespace SlidingPieceUtils {
    uint64_t get_bishop_attacks(int idx, uint64_t occupancy);
    uint64_t get_rook_attacks(int idx, uint64_t occupancy);
    uint64_t get_queen_attacks(int idx, uint64_t occupancy);
}