#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"

enum class MovePickerStage {
    TT_MOVE,

    GEN_NOISY,
    PICK_GOOD_NOISY,

    KILLER,
    GEN_QUIET,
    PICK_QUIET,

    GEN_BAD_NOISY,
    PICK_BAD_NOISY,

    QSEARCH_GEN_NOISY,
    QSEARCH_PICK_GOOD_NOISY,

    NONE
};

class MovePicker {
    private:
        const bool is_quiescence;
        Move tt_move, killer_move;
        const Position& pos;
        HistoryTable& hist_table;
        BoardHistory& board_hist;

        MoveList moves;
        MoveList bad_noisies;
        size_t idx;
        MovePickerStage stage;
        i32 depth;

        auto gen_noisies() -> void;
        auto score_noisies() -> void;
        auto pick_good_noisies() -> std::optional<ScoredMove>;

        auto gen_quiets() -> void;
        auto score_quiets() -> void;
        auto pick_move(MoveList& moves) -> std::optional<ScoredMove>;

    public:
        MovePicker(bool is_quiescence, Move tt_move, Move killer_move, const Position& pos, HistoryTable& hist_table, BoardHistory& board_hist, i32 depth)
            : is_quiescence(is_quiescence),
              tt_move(tt_move),
              killer_move(killer_move),
              pos(pos),
              hist_table(hist_table),
              board_hist(board_hist),
              depth(depth) {
                idx = 0;
                stage = [&] {
                    if (tt_move.is_null_move()) {
                        return MovePickerStage::GEN_NOISY;
                    } else if (is_quiescence) {
                        return MovePickerStage::QSEARCH_GEN_NOISY;
                    } else {
                        return MovePickerStage::TT_MOVE;
                    }
                }();
              }
        std::optional<ScoredMove> next(const bool skip_quiets);
};