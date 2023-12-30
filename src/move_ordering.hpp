#pragma once

#include "chessboard.hpp"
#include "move.hpp"

namespace MoveOrdering {
    void reorder_moves(MoveList& moves, const ChessBoard& board, const Move pv_move, bool& found_pv_move, const std::array<uint32_t, 8192>& history_table, const std::array<Move, KILLER_COUNT>& killers);
} // namespace MoveOrdering