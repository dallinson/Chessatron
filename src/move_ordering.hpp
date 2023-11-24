#pragma once

#include "chessboard.hpp"
#include "move.hpp"

namespace MoveOrdering {
    void reorder_moves(MoveList& moves, const ChessBoard& board, const Move pv_move, bool& found_pv_move);
} // namespace MoveOrdering