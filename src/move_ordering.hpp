#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"

class MovePicker {
    private:
        MoveList moves;
        size_t idx;

    public:
        MovePicker(MoveList&& input_moves, const ChessBoard& board, const Move pv_move, HistoryTable& history_table, Move killer, bool& found_pv_move);
        std::optional<ScoredMove> next();

        std::span<const ScoredMove> evaluated_moves() const { assert(idx > 0); return std::span(moves.begin(), idx - 1); };

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};