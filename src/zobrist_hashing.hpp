#pragma once

#include <array>
#include <functional>

#include "magic_numbers.hpp"
#include "pieces.hpp"
#include "utils.hpp"

constexpr inline ZobristKey calculate_zobrist_key(Piece piece, uint8_t pos) { return (piece.to_bitboard_idx() * 64) + pos; };
constexpr inline ZobristKey calculate_zobrist_key(Piece piece, Square sq) { return calculate_zobrist_key(piece, sq_to_int(sq)); };

namespace ZobristKeys {
    extern const std::array<ZobristKey, 64 * 12> PositionKeys;
    extern const ZobristKey SideToMove;
    extern const std::array<ZobristKey, 10> EnPassantKeys;
    extern const std::array<ZobristKey, 4> CastlingKeys;
    extern const std::array<Bitboard, 16> EnPassantCheckBitboards;
} // namespace ZobristKeys
