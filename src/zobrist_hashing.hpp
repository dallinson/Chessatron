#pragma once

#include <array>
#include <functional>

#include "magic_numbers.hpp"
#include "utils.hpp"

#define ZOBRIST_POSITION_KEY(piece, position) (((piece).to_bitboard_idx() * 64) + (position))

namespace ZobristKeys {
    extern const std::array<ZobristKey, 64 * 12> PositionKeys;
    extern const ZobristKey SideToMove;
    extern const std::array<ZobristKey, 10> EnPassantKeys;
    extern const std::array<ZobristKey, 4> CastlingKeys;
    extern const std::array<Bitboard, 16> EnPassantCheckBitboards;
} // namespace ZobristKeys
