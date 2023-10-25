#include "evaluation.hpp"

#include <bit>

#include "move_generator.hpp"

constexpr std::array<int32_t, 6> Evaluation::PieceScores = {100, 500, 320, 330, 900, 20000};

Score Evaluation::evaluate_board(const ChessBoard& c, const Side side) {
    auto legal_move_count = MoveGenerator::generate_legal_moves(c, side).len();
    if (legal_move_count == 0) {
        if (c.get_checkers(side) != 0) {
            return MagicNumbers::NegativeInfinity;
        } else {
            return 0;
        }
        // we don't want to be checkmated, but we also want to avoid
    }
    return (((std::popcount(c.get_pawn_occupancy(side)) * get_piece_score(PieceTypes::PAWN)) +
             (std::popcount(c.get_rook_occupancy(side)) * get_piece_score(PieceTypes::ROOK)) +
             (std::popcount(c.get_knight_occupancy(side)) * get_piece_score(PieceTypes::KNIGHT)) +
             (std::popcount(c.get_bishop_occupancy(side)) * get_piece_score(PieceTypes::BISHOP)) +
             (std::popcount(c.get_queen_occupancy(side)) * get_piece_score(PieceTypes::QUEEN))) +
            (legal_move_count * MOBILITY_WEIGHT));
}

Score Evaluation::evaluate_board(const ChessBoard& c) {
    const Score side_to_move_score = evaluate_board(c, c.get_side_to_move());
    if (side_to_move_score == MagicNumbers::NegativeInfinity) {
        return MagicNumbers::NegativeInfinity;
    }
    const Score enemy_side_score = evaluate_board(c, ENEMY_SIDE(c.get_side_to_move()));
    if (enemy_side_score == MagicNumbers::NegativeInfinity) {
        return MagicNumbers::PositiveInfinity;
    }
    return side_to_move_score - enemy_side_score;
}