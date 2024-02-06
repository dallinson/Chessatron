#pragma once

#include "chessboard.hpp"
#include "move.hpp"

class MovePicker {
    private:
        size_t sorted_idx;
        MoveList move_list;
    public:
        MovePicker(MoveList&& moves, const ChessBoard& board, const Move pv_move, std::array<int32_t, 8192>& history_table, Move killer_move, bool& found_pv_move);
        ScoredMove next_move();
        size_t size();
};