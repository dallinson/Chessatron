#pragma once

#include "move.hpp"
#include "chessboard.hpp"

namespace MoveOrdering {
    size_t reorder_captures(MoveList& move_list, const ChessBoard& c);
}