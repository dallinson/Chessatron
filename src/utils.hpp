#pragma once

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>

typedef uint64_t Bitboard;
typedef uint64_t ZobristKey;
typedef int16_t Score;

enum class Side : uint8_t {
    WHITE = 0,
    BLACK = 1,
};

constexpr inline uint64_t bit(uint8_t x) { return ((uint64_t) 1) << x; };
template <std::integral T> constexpr inline T get_bit(T val, uint8_t x) { return (val >> x) & 0x1; };
template <std::integral T> constexpr inline void set_bit(T& val, uint8_t x) { val |= bit(x); };
template <std::integral T> constexpr inline void clear_bit(T& val, uint8_t x) { val &= ~bit(x); };
template <std::integral T> constexpr inline void toggle_bit(T& val, uint8_t x) { val ^= bit(x); };

constexpr inline uint64_t bits(uint8_t max, uint8_t min) { return ((0xFFFFFFFFFFFFFFFF << (min)) & (0xFFFFFFFFFFFFFFFF >> (64 - ((max) + 1)))); };
template <std::integral T> constexpr inline T get_bits(T val, uint8_t max, uint8_t min) { return (val & bits(max, min)) >> min; };
constexpr inline uint8_t get_rank(uint8_t pos) { return get_bits(pos, 5, 3); };
constexpr inline uint8_t get_file(uint8_t pos) { return get_bits(pos, 2, 0); };
// rank is the row and file the column

constexpr inline uint8_t get_position(uint8_t rank, uint8_t file) { return ((rank & 0x7) << 3) | (file & 0x7); };
#define ENEMY_SIDE(side) (((side) == Side::WHITE) ? Side::BLACK : Side::WHITE)

void print_bitboard(Bitboard to_print);

constexpr inline int get_lsb(Bitboard bitboard) { return std::countr_zero(bitboard); };

constexpr inline Bitboard idx_to_bitboard(int idx) { return bit(idx); };

constexpr inline int pop_min_bit(Bitboard& num) {
    const int to_return = get_lsb(num);
    clear_bit(num, to_return);
    return to_return;
};

bool is_aligned(int sq_1, int sq_2, int sq_3);