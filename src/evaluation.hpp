#pragma once

#include <array>
#include <cstdint>

#include "magic_numbers/piece_square_tables.hpp"
#include "chessboard.hpp"
#include "utils.hpp"

#define MOBILITY_WEIGHT 10
// the number of centipawns each possible move is worth

namespace Evaluation {
    Score evaluate_board(const Position& c);
} // namespace Evaluation