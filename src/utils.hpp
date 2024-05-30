#pragma once

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>

using Bitboard = uint64_t;
using ZobristKey = uint64_t;
using Score = int16_t;

constexpr int MAX_TURN_MOVE_COUNT = 218;
constexpr int MAX_GAME_MOVE_COUNT = 5899;
// the maximum possible number of moves, currently 218 on position R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1

enum class Side : uint8_t {
    WHITE = 0,
    BLACK = 1,
};

enum class Square: uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
};

constexpr inline uint64_t bit(uint8_t x) { return ((uint64_t) 1) << x; };
template <std::integral T> constexpr inline T get_bit(T val, uint8_t x) { return (val >> x) & 0x1; };
template <std::integral T> constexpr inline void set_bit(T& val, uint8_t x) { val |= bit(x); };
template <std::integral T> constexpr inline void clear_bit(T& val, uint8_t x) { val &= ~bit(x); };
template <std::integral T> constexpr inline void toggle_bit(T& val, uint8_t x) { val ^= bit(x); };

constexpr inline uint64_t bits(uint8_t max, uint8_t min) { return ((0xFFFFFFFFFFFFFFFF << (min)) & (0xFFFFFFFFFFFFFFFF >> (64 - ((max) + 1)))); };
template <std::integral T> constexpr inline T get_bits(T val, uint8_t max, uint8_t min) { return (val & bits(max, min)) >> min; };
constexpr inline uint8_t rank(uint8_t pos) { return get_bits(pos, 5, 3); };
constexpr inline uint8_t file(uint8_t pos) { return get_bits(pos, 2, 0); };
// rank is the row and file the column
constexpr Bitboard rank_bb(uint8_t rnk) { return static_cast<Bitboard>(0x00000000000000FF) << (8 * rnk); };
constexpr Bitboard file_bb(uint8_t fil) { return 0x0101010101010101 << fil; };

constexpr inline uint8_t get_position(uint8_t rank, uint8_t file) { return ((rank & 0x7) << 3) | (file & 0x7); };
constexpr inline Side enemy_side(Side stm) { return (stm == Side::WHITE) ? Side::BLACK : Side::WHITE; };

void print_bb(Bitboard to_print);

constexpr inline int get_lsb(Bitboard bitboard) { return std::countr_zero(bitboard); };

constexpr inline Bitboard idx_to_bb(int idx) { return bit(idx); };

constexpr inline int pop_lsb(Bitboard& num) {
    const int to_return = get_lsb(num);
    clear_bit(num, to_return);
    return to_return;
};

bool is_aligned(int sq_1, int sq_2, int sq_3);