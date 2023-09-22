#pragma once

#include <bit>
#include <cstdint>
#include <immintrin.h>

typedef uint64_t Bitboard;

#define BIT(x) (((uint64_t) 1) << (x))
#define GET_BIT(val, x) (((val) >> (x)) & 0x1)
#define SET_BIT(val, x) val |= BIT(x)
#define CLEAR_BIT(val, x) val &= ~BIT(x)
#define TOGGLE_BIT(val, x) val ^= BIT(x)

#define BITS(max, min) ((0xFFFFFFFFFFFFFFFF << (min)) & (0xFFFFFFFFFFFFFFFF >> (64 - ((max) + 1))))
#define GET_BITS(val, max, min) (((val) &BITS(max, min)) >> (min))
#define GET_RANK(val) GET_BITS(val, 5, 3)
#define GET_FILE(val) GET_BITS(val, 2, 0)

#define POSITION(rank, file) ((((rank) &0x7) * 8) + ((file) &0x7))

void print_bitboard(Bitboard to_print);

inline int bitboard_to_idx(Bitboard bitboard) {
    return std::countr_zero(bitboard);
};

inline Bitboard idx_to_bitboard(int idx) {
    return BIT(idx);
};

int pop_min_bit(uint64_t* num);

namespace SlidingPieceUtils {
    Bitboard get_bishop_attacks(int idx, Bitboard occupancy);
    Bitboard get_rook_attacks(int idx, Bitboard occupancy);
    Bitboard get_queen_attacks(int idx, Bitboard occupancy);
} // namespace SlidingPieceUtils