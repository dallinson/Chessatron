#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"

class MovePicker {
    private:
        MoveList moves;
        size_t start_idx;
        MovePickerStage stage = MovePickerStage::NONE;

    public:
        MovePicker(MoveList&& input_moves, const Position& pos, const BoardHistory& hist, const Move pv_move, const HistoryTable& history_table, Move killer);
        std::optional<ScoredMove> next(const bool skip_quiets);
};