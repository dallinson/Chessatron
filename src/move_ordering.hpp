#pragma once

#include "chessboard.hpp"
#include "move.hpp"

class MovePicker {
    private:
        MoveList moves;
        size_t idx;

    public:
        MovePicker(MoveList&& input_moves, const ChessBoard& board, const Move pv_move, std::array<int32_t, 8192>& history_table, Move killer, bool& found_pv_move);
        std::optional<ScoredMove> next();

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};