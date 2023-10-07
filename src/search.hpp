#pragma once

#include <cstdint>
#include "chessboard.hpp"

namespace Perft {
    uint64_t run_perft(ChessBoard& c, int depth, bool print_debug = false);
}