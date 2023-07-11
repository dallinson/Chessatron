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

class Piece {
    private:
    uint_fast8_t val;
    public:

    Piece() : val(0) {};
    Piece(uint_fast8_t val) : val(val) {};

    void set_value(uint_fast8_t val) { this->val = val; };
    uint_fast8_t get_value() const { return this->val; };
    uint_fast8_t get_piece_val() const { return GET_BITS(val, 2, 0); };
    uint_fast8_t get_side() const { return GET_BIT(val, 3); };

    uint_fast8_t to_bitboard_idx() const { return (get_piece_val() - 1) + get_side();};
};