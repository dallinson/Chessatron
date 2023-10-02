#pragma once

#include <array>
#include <functional>

#include "utils.hpp"
#include "chessboard.hpp"
#include "magic_numbers.hpp"

#define ZOBRIST_POSITION_KEY(piece, position) (((position) * 64) + (piece).get_value())

namespace ZobristKeys {
    extern const std::array<ZobristKey, 64*12> PositionKeys;
    extern const ZobristKey SideToMove;
    extern const std::array<ZobristKey, 10> EnPassantKeys;
    extern const std::array<ZobristKey, 4> CastlingKeys;
}


template<> struct std::hash<ChessBoard> {
    std::size_t operator()(const ChessBoard& c) const {
        return c.get_zobrist_key();
    }
};