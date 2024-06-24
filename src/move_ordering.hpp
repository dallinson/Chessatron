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
        MovePicker(const Position& pos, const BoardHistory& board_hist, const HistoryTable& hist_table, const Move tt_move, const Move killer_move, const bool is_qsearch);
        std::optional<ScoredMove> next(const bool skip_quiets);

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};