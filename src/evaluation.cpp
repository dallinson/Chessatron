#include "evaluation.hpp"

#include <bit>

#include "move_generator.hpp"
#include "magic_numbers.hpp"
#include "search.hpp"

template <bool is_endgame> Score Evaluation::evaluate_board(const ChessBoard& board, const Side side) {
    if constexpr (is_endgame) {
        return board.get_eg_score(side);
    } else {
        return board.get_mg_score(side);
    }
}

Score Evaluation::evaluate_board(ChessBoard& board) { 
    const Side enemy = enemy_side(board.get_side_to_move()); 
    const Score mg_score = evaluate_board<false>(board, board.get_side_to_move()) - evaluate_board<false>(board, enemy);
    const Score eg_score = evaluate_board<true>(board, board.get_side_to_move()) - evaluate_board<true>(board, enemy);

    const auto midgame_phase = std::min(board.get_midgame_phase(), (uint8_t) 24);
    const auto endgame_phase = 24 - midgame_phase;
    return std::clamp(((midgame_score * midgame_phase) + (endgame_score * endgame_phase)) / 24,
        MagicNumbers::NegativeInfinity + MAX_PLY + 1,
        MagicNumbers::PositiveInfinity - MAX_PLY - 1);
}