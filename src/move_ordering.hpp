#pragma once

#include "chessboard.hpp"
#include "move.hpp"

namespace MoveOrdering {
    size_t reorder_captures(MoveList& move_list, const ChessBoard& c, size_t start_pos);
    bool reorder_pv_move(MoveList& move_list, const Move pv_move);
} // namespace MoveOrdering