#pragma once

#include <array>
#include <cstdint>

#include "chessboard.hpp"

typedef int32_t Score;
#define MOBILITY_WEIGHT 10
// the number of centipawns each possible move is worth

namespace Evaluation {
    extern const std::array<Score, 6> PieceScores;
    constexpr Score get_piece_score(PieceTypes p) { return PieceScores[static_cast<int>(p) - 1]; };
    Score evaluate_board(const ChessBoard& c);
    Score evaluate_board(const ChessBoard& c, const Side side);
} // namespace Evaluation