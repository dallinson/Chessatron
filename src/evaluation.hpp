#pragma once

#include <array>
#include <cstdint>

#include "magic_numbers/positional_scores.hpp"
#include "chessboard.hpp"
#include "utils.hpp"

#define MOBILITY_WEIGHT 10
// the number of centipawns each possible move is worth

namespace Evaluation {

    constexpr std::array<Score, 6> MidgameScores = { 82, 477, 337, 365, 1025, 0 };
    constexpr std::array<Score, 6> EndgameScores = { 94, 512, 281, 297, 936, 0 };
    template <bool is_endgame>
    constexpr Score get_piece_score(PieceTypes p) { 
        const auto piece_idx = static_cast<uint8_t>(p) - 1;
        if constexpr(is_endgame) {
            return EndgameScores[piece_idx];
        } else {
            return MidgameScores[piece_idx];
        }
    };

    Score evaluate_board(ChessBoard& c);
    template <bool is_endgame>
    Score evaluate_board(const ChessBoard& c, const Side side);

    uint8_t calculate_game_phase(const ChessBoard& board);
} // namespace Evaluation