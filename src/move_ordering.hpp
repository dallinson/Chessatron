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
        const ChessBoard& board;
        const BoardHistory& board_hist;
        const HistoryTable& hist_table;

        const bool is_qsearch;

        const Move tt_move;
        const Move killer_move;

        MovePickerStage stage = MovePickerStage::PICK_REMAINING;

        MoveList moves;
        size_t idx;

        void score_moves();

    public:
        MovePicker(const ChessBoard& board, const BoardHistory& board_hist, const HistoryTable& hist_table, const bool is_qsearch, const Move tt_move, const Move killer_move) :
            board(board),
            board_hist(board_hist),
            hist_table(hist_table),

            is_qsearch(is_qsearch),

            tt_move(tt_move),
            killer_move(killer_move) {
                idx = 0;
                if (is_qsearch) {
                    stage = MovePickerStage::GEN_QSEARCH;
                } else {
                    if (!tt_move.is_null_move()) {
                        stage = MovePickerStage::PICK_TT;
                    } else {
                        stage = MovePickerStage::GEN_ALL;
                    }
                }
                assert(stage != MovePickerStage::PICK_REMAINING);
            }

        MovePicker(MoveList&& input_moves, const ChessBoard& board, const BoardHistory& hist, const Move pv_move, HistoryTable& history_table, Move killer);
        std::optional<ScoredMove> next(const bool skip_quiets);

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};