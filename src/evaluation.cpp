#include "evaluation.hpp"

#include <bit>

#include "move_generator.hpp"
#include "magic_numbers.hpp"
#include "search.hpp"

int32_t get_eg_score(int32_t score) {
    return std::bit_cast<int16_t>(static_cast<uint16_t>(std::bit_cast<uint32_t>(score + 0x8000) >> 16));
}

int32_t get_mg_score(int32_t score) {
    return std::bit_cast<int16_t>(static_cast<uint16_t>(score));
}

Score Evaluation::evaluate_board(ChessBoard& board) { 
    const Side stm = board.get_side_to_move();
    const Side enemy = enemy_side(board.get_side_to_move()); 
    const auto mg_score = get_mg_score(board.get_score(stm)) - get_mg_score(board.get_score(enemy));
    const auto eg_score = get_eg_score(board.get_score(stm)) - get_eg_score(board.get_score(enemy));

    const auto mg_phase = std::min(board.get_mg_phase(), (uint8_t) 24);
    const auto eg_phase = 24 - mg_phase;

    return std::clamp(((mg_score * mg_phase) + (eg_score * eg_phase)) / 24,
        MagicNumbers::NegativeInfinity + MAX_PLY + 1,
        MagicNumbers::PositiveInfinity - MAX_PLY - 1);
}