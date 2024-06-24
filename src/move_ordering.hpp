#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"
#include "move_generator.hpp"

enum class MovePickerStage {
    PICK_TT,
    GEN_QSEARCH,
    GEN_ALL,
    PICK_REMAINING,
};

class MovePicker {
    private:
        const Position& pos;
        const BoardHistory& board_hist;
        const HistoryTable& hist_table;

        const Move tt_move, killer_move;
        const bool is_qsearch;

        MovePickerStage stage;
        MoveList moves;
        size_t idx;

        void score_moves();
        ScoredMove partial_selection_sort();
    public:
        MovePicker(const Position& pos, const BoardHistory& board_hist, const HistoryTable& hist_table, const Move tt_move, const Move killer_move, const bool is_qsearch)
            : pos(pos),
              board_hist(board_hist),
              hist_table(hist_table),
              tt_move(tt_move),
              killer_move(killer_move),
              is_qsearch(is_qsearch),
              idx(0)
            {
                if (is_qsearch) {
                    stage = MovePickerStage::GEN_QSEARCH;
                } else {
                    if (tt_move.is_null_move()) {
                        stage = MovePickerStage::GEN_ALL;
                    } else {
                        stage = MovePickerStage::PICK_TT;
                    }
                }
            }

        std::optional<ScoredMove> next(const bool skip_quiets);

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};