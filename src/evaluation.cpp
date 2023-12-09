#include "evaluation.hpp"

#include <bit>

#include "move_generator.hpp"

template <bool is_endgame> Score Evaluation::evaluate_board(const ChessBoard& board, const Side side) {
    if constexpr (is_endgame) {
        return board.get_endgame_score(side);
    } else {
        return board.get_midgame_score(side);
    }
}

Score Evaluation::evaluate_board(ChessBoard& board) { 
    const Side enemy_side = ENEMY_SIDE(board.get_side_to_move()); 
    const Score midgame_score = evaluate_board<false>(board, board.get_side_to_move()) - evaluate_board<false>(board, enemy_side);
    const Score endgame_score = evaluate_board<true>(board, board.get_side_to_move()) - evaluate_board<true>(board, enemy_side);

    const auto midgame_phase = std::min(board.get_midgame_phase(), (uint8_t) 24);
    const auto endgame_phase = 24 - midgame_phase;
    return ((midgame_score * midgame_phase) + (endgame_score * endgame_phase)) / 24;
}

uint8_t Evaluation::calculate_game_phase(const ChessBoard& board) {
    return std::popcount(board.get_knight_occupancy())
        + std::popcount(board.get_bishop_occupancy())
        + 2 * std::popcount(board.get_rook_occupancy())
        + 4 * std::popcount(board.get_queen_occupancy());
}