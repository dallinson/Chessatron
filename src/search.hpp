#pragma once

#include "chessboard.hpp"
#include <cstdint>

namespace Perft {
    uint64_t run_perft(ChessBoard& c, int depth, bool print_debug = false);
}

namespace Search {
    Move select_random_move(const ChessBoard& c);
    Move run_negamax(ChessBoard& c, MoveHistory& m, int depth = 4);
}