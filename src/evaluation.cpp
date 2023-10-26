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

/**
 * @brief Determines if the board c is in an endgame as determined by the
 * Simplified Evaluation Function
 *
 * @param c The board to test for endgame
 * @return true if the board is in endgame
 * @return false
 */
bool Evaluation::is_endgame(const ChessBoard& c) {
    if (c.get_queen_occupancy() == 0) {
        // If neither side has a queen the board is in endgame
        return true;
    }
    for (Side side : {Side::WHITE, Side::BLACK}) {
        if (c.get_queen_occupancy(side) != 0) {
            // If this side has a queen
            const Bitboard minor_pieces = c.get_bishop_occupancy(side) | c.get_knight_occupancy(side);
            if (std::popcount(minor_pieces) <= 1) {
                // If the side with the queen has at most 1 minor piece
                const Bitboard king_queen_occupancy = c.get_king_occupancy(side) | c.get_queen_occupancy(side);
                const Bitboard without_monarch_occupancy = c.get_occupancy(side) ^ king_queen_occupancy;
                if (without_monarch_occupancy != minor_pieces) {
                    return false;
                    // The side with a queen can have at most one minor piece and no other pieces
                    // It always has a king and must have a queen so we ensure that the pieces that _aren't_
                    // king/queen are equivalent to the minor pieces
                }
            } else {
                return false;
            }
        }
    }
    return true;
}

template <PieceTypes piece> Score adjust_positional_value(const ChessBoard& c, const Side side) {
    Bitboard occupancy;
   // std::array<Score, 64> position_scores;
    switch (piece) {
    case PieceTypes::PAWN:
        occupancy = c.get_pawn_occupancy(side);
        break;
    case PieceTypes::KNIGHT:
        occupancy = c.get_knight_occupancy(side);
        break;
    case PieceTypes::BISHOP:
        occupancy = c.get_bishop_occupancy(side);
        break;
    case PieceTypes::ROOK:
        occupancy = c.get_rook_occupancy(side);
        break;
    case PieceTypes::QUEEN:
        occupancy = c.get_queen_occupancy(side);
        break;
    case PieceTypes::KING:
        occupancy = c.get_king_occupancy(side);
        break;
    }
    Score to_return = 0;
    while (occupancy) {
   //     int piece_idx = pop_min_bit(occupancy);
    }
    return to_return;
}

Score adjust_positional_value(const ChessBoard& c, const Side side) {
    return adjust_positional_value<PieceTypes::PAWN>(c, side) + adjust_positional_value<PieceTypes::KNIGHT>(c, side) +
           adjust_positional_value<PieceTypes::BISHOP>(c, side) + adjust_positional_value<PieceTypes::ROOK>(c, side) +
           adjust_positional_value<PieceTypes::QUEEN>(c, side) + adjust_positional_value<PieceTypes::KING>(c, side);
}