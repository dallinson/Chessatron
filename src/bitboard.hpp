#pragma once

#include <cassert>
#include <cstdint>

#include "utils.hpp"

struct Bitboard {
    public:
        uint64_t bb;

    public:
        constexpr Bitboard() : bb(0) {};
        constexpr Bitboard(uint64_t bb) : bb(bb) {};
        constexpr Bitboard(Square sq) : bb(bit(sq_to_int(sq))) {};

        constexpr operator uint64_t() const { return bb; };
        constexpr bool empty() const { return bb == 0; };
        constexpr bool operator[](const int idx) const { return get_bit(bb, idx); };
        constexpr bool operator[](const Square sq) const { return get_bit(bb, sq_to_int(sq)); };
        constexpr Square lsb() const { return static_cast<Square>(std::countr_zero(bb)); };
        constexpr Square pop_lsb() {
            assert(!empty());
            const auto to_return = lsb();
            bb &= (bb - 1);
            return to_return;
        };
        constexpr int popcnt() const { return std::popcount(bb); };

        constexpr Bitboard operator~() const { return ~bb; };
        constexpr Bitboard operator&(const Bitboard other) const { return bb & other.bb; };
        constexpr Bitboard operator|(const Bitboard other) const { return bb | other.bb; };
        constexpr Bitboard operator^(const Bitboard other) const { return bb ^ other.bb; };
        constexpr void operator&=(const Bitboard other) { bb &= other.bb; };
        constexpr void operator|=(const Bitboard other) { bb |= other.bb; };
        constexpr void operator^=(const Bitboard other) { bb ^= other.bb; };

        constexpr Bitboard operator&(const Square sq) const { return bb & Bitboard(sq).bb; };
        constexpr Bitboard operator|(const Square sq) const { return bb | Bitboard(sq).bb; };
        constexpr Bitboard operator^(const Square sq) const { return bb ^ Bitboard(sq).bb; };
        constexpr void operator&=(const Square sq) { bb &= Bitboard(sq).bb; };
        constexpr void operator|=(const Square sq) { bb |= Bitboard(sq).bb; };
        constexpr void operator^=(const Square sq) { bb ^= Bitboard(sq).bb; };

        constexpr Bitboard operator<<(const int offset) const { return bb << offset; };
        constexpr Bitboard operator>>(const int offset) const { return bb >> offset; };

        constexpr bool operator==(const Bitboard other) const { return bb == other.bb; };
};

constexpr Bitboard rank_bb(uint8_t rnk) { return static_cast<Bitboard>(0x00000000000000FF) << (8 * rnk); };
constexpr Bitboard file_bb(uint8_t fil) { return 0x0101010101010101 << fil; };