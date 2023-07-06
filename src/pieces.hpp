#pragma once

#include "utils.hpp"

#define SIDE_MASK BIT(3)
#define PIECE_MASK BITS(2, 0)

#define BLACK BIT(3)

#define WHITE_IDX 0
#define BLACK_IDX 1

#define KING_VALUE 6
#define QUEEN_VALUE 5
#define BISHOP_VALUE 4
#define KNIGHT_VALUE 3
#define ROOK_VALUE 2
#define PAWN_VALUE 1