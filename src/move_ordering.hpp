#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"
#include "search.hpp"

class MovePicker {
    private:
        MoveList moves;
        size_t idx;

    public:
        MovePicker(MoveList&& input_moves, const Position& pos, const BoardHistory& hist, const Move pv_move, const HistoryTable& history_table, const KillerMoves& killers);
        std::optional<ScoredMove> next(const bool skip_quiets);
};