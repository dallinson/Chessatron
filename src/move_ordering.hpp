#pragma once

#include <span>

#include "chessboard.hpp"
#include "history.hpp"
#include "move.hpp"
#include "move_generator.hpp"

class MovePicker {
    private:
        const bool _is_qsearch;
        const Position& _pos;
        const BoardHistory& _board_hist;
        const Move _tt_move;
        const HistoryTable& _history_table;
        const Move _killer_move;

        MoveList moves;
        size_t idx;

        auto score_moves() -> void;

    public:
        MovePicker(MoveList&& input_moves, const Position& pos, const BoardHistory& hist, const Move pv_move, const HistoryTable& history_table, Move killer);
        MovePicker(const bool is_qsearch, const Position& pos, const BoardHistory& board_hist, const Move tt_move, const HistoryTable& history_table, const Move killer_move)
            : _is_qsearch(is_qsearch),
              _pos(pos),
              _board_hist(board_hist),
              _tt_move(tt_move),
              _history_table(history_table),
              _killer_move(killer_move) {
                if (_is_qsearch && !_pos.in_check()) {
                    moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIESCENCE>(_pos, _pos.stm());
                } else {
                    moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(_pos, _pos.stm());
                }
                idx = 0;
                score_moves();
              };
        std::optional<ScoredMove> next(const bool skip_quiets);

        const ScoredMove& operator[](size_t idx) { return moves[idx]; };
};