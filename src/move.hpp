#pragma once

#include <cstdint>
#include <cstring>
#include <array>

#include "utils.hpp"

#define MAX_MOVE_COUNT 218
// the maximum possible number of moves, currently 218 on position R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1

enum MoveFlags : uint_fast8_t {
    QUIET_MOVE = 0,
    DOUBLE_PAWN = 1,
    KINGSIDE_CASTLE = 2,
    QUEENSIDE_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT_CAPTURE = 5,
    ROOK_PROMOTION = 8,
    KNIGHT_PROMOTION = 9,
    BISHOP_PROMOTION = 10,
    QUEEN_PROMOTION = 11,
    ROOK_PROMOTION_CAPTURE = 12,
    KNIGHT_PROMOTION_CAPTURE = 13,
    BISHOP_PROMOTION_CAPTURE = 14,
    QUEEN_PROMOTION_CAPTURE = 15,
};

class Move {
    private:
    uint_fast16_t move;

    public:


    Move() : move(0) {};
    Move(uint_fast16_t v) : move(v) {};

    uint_fast16_t get_move() { return move; };
    uint_fast8_t get_src_square() { return GET_BITS(move, 5, 0); };
    uint_fast8_t get_dest_square() { return GET_BITS(move, 11, 6); };

    uint_fast8_t get_src_rank() { return GET_BITS(move, 5, 3); };
    uint_fast8_t get_src_file() { return GET_BITS(move, 2, 0); };

    uint_fast8_t get_dest_src_rank() { return GET_BITS(move, 11, 9); };
    uint_fast8_t get_dest_file() { return GET_BITS(move, 8, 6); };

    MoveFlags get_move_flags() { return (MoveFlags) GET_BITS(move, 15, 12); };

};

class MoveList: private std::array<Move, MAX_MOVE_COUNT> {
    private:
    size_t idx;

    public:
    MoveList() : idx(0) {};
    using std::array<Move, MAX_MOVE_COUNT>::operator[];
    
    void add_move(const Move to_add) { this->data()[idx] = to_add; idx += 1; };
    void add_moves(const MoveList& other_list) { size_t other_len = other_list.len(); memcpy(&this->data()[idx], other_list.data(), other_len * sizeof(Move)); idx += other_len; };

    size_t len() const { return this->idx; };
};