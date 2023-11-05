#pragma once

#include "chessboard.hpp"
#include "move.hpp"

namespace MoveOrdering {
    size_t reorder_captures_first(MoveList& move_list, size_t start_pos);
    void sort_captures_mvv_lva(MoveList& move_list, const ChessBoard& c, const size_t capture_start, const size_t capture_count);
    bool reorder_pv_move(MoveList& move_list, const Move pv_move);
} // namespace MoveOrdering