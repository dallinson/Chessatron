#pragma once

#include <array>
#include <cstdint>

#include "magic_numbers/piece_square_tables.hpp"
#include "chessboard.hpp"
#include "utils.hpp"

#define MOBILITY_WEIGHT 10
// the number of centipawns each possible move is worth

namespace Evaluation {

    template <bool is_endgame>
    constexpr Score get_piece_score(PieceTypes p) { 
        const auto piece_idx = static_cast<uint8_t>(p) - 1;
        if constexpr (is_endgame) {
            return PieceSquareTables::EndgameScores[piece_idx];
        } else {
            return PieceSquareTables::MidgameScores[piece_idx];
        }
    };

    Score evaluate_board(ChessBoard& c);
    template <bool is_endgame>
    Score evaluate_board(const ChessBoard& c, const Side side);
} // namespace Evaluation