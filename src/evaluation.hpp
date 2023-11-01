#pragma once

#include <array>
#include <cstdint>

#include "chessboard.hpp"

typedef int32_t Score;
#define MOBILITY_WEIGHT 10
// the number of centipawns each possible move is worth

namespace PieceSquareTables {
    extern const std::array<Score, 64> PawnScores;
    extern const std::array<Score, 64> RookScores;
    extern const std::array<Score, 64> BishopScores;
    extern const std::array<Score, 64> KnightScores;
    extern const std::array<Score, 64> QueenScores;
    extern const std::array<Score, 64> KingMidgameScores;
    extern const std::array<Score, 64> KingEndgameScores;
} // namespace PieceSquareTables

namespace Evaluation {

    extern const std::array<Score, 6> PieceScores;
    constexpr Score get_piece_score(PieceTypes p) { return PieceScores[static_cast<int>(p) - 1]; };

    Score evaluate_board(const ChessBoard& c);
    Score evaluate_board(const ChessBoard& c, const Side side);

    bool is_endgame(const ChessBoard& c);

    template <PieceTypes piece> Score adjust_positional_value(const ChessBoard& c, const Side s);
    Score adjust_positional_value(const ChessBoard& c, const Side s);
} // namespace Evaluation