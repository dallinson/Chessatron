#include "utils.hpp"

#include <cstdio>

#include "magic_numbers.hpp"

bool is_aligned(Square sq_1, Square sq_2, Square sq_3) { return !(MagicNumbers::AlignedSquares[sq_to_int(sq_1)][sq_to_int(sq_2)] & sq_3).empty(); }