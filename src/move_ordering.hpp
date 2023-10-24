#pragma once

#include "chessboard.hpp"
#include "move.hpp"

namespace MoveOrdering {
    size_t reorder_captures(MoveList& move_list, const ChessBoard& c);
}