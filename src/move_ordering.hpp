#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"

class MovePicker {
    private:
        MoveList moves;
        MoveList legals;
        size_t idx;

    public:
        MovePicker(MoveList&& input_moves, const ChessBoard& board, const BoardHistory& hist, const Move pv_move, HistoryTable& history_table, Move killer, bool& found_pv_move);
        std::optional<ScoredMove> next_legal(const ChessBoard& board);

        std::span<const ScoredMove> evaluated_moves() const { assert(legals.size() > 0); return std::span(legals.begin(), legals.size() - 1); };

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};