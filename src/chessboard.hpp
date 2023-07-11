#pragma once

#include <array>
#include <cstdint>

#include "pieces.hpp"
#include "utils.hpp"

#define KING_OFFSET (2 * ((KING_VALUE) -1))
#define QUEEN_OFFSET (2 * ((QUEEN_VALUE) -1))
#define BISHOP_OFFSET (2 * ((BISHOP_VALUE) -1))
#define KNIGHT_OFFSET (2 * ((KNIGHT_VALUE) -1))
#define ROOK_OFFSET (2 * ((ROOK_VALUE) -1))
#define PAWN_OFFSET (2 * ((PAWN_VALUE) -1))

#define QUEEN_SCORE 9
#define ROOK_SCORE 5
#define BISHOP_SCORE 3
#define KNIGHT_SCORE 3
#define PAWN_SCORE 1

class ChessBoard {
  private:
    std::array<uint64_t, 12> bitboards;

    std::array<Piece, 64> pieces;

    inline uint64_t get_pair_occupancy(int offset) const { return bitboards[offset] | bitboards[offset + 1]; };

  public:
    uint64_t get_occupancy() const { return get_king_occupancy() | get_queen_occupancy() | get_bishop_occupancy() | get_knight_occupancy() | get_rook_occupancy() | get_pawn_occupancy(); };
    uint64_t get_occupancy(int side) const { return get_side_occupancy(side); };

    uint64_t get_side_occupancy(int side) const { return bitboards[KING_OFFSET + side] | bitboards[QUEEN_OFFSET + side] | bitboards[BISHOP_OFFSET + side] | bitboards[KNIGHT_OFFSET + side] | bitboards[ROOK_OFFSET + side] | bitboards[PAWN_OFFSET + side]; };
    uint64_t get_white_occupancy() const { return get_side_occupancy(WHITE_IDX); };
    uint64_t get_black_occupancy() const { return get_side_occupancy(BLACK_IDX); };

    uint64_t get_king_occupancy() const { return get_pair_occupancy(KING_OFFSET); };
    uint64_t get_queen_occupancy() const { return get_pair_occupancy(QUEEN_OFFSET); };
    uint64_t get_bishop_occupancy() const { return get_pair_occupancy(BISHOP_OFFSET); };
    uint64_t get_knight_occupancy() const { return get_pair_occupancy(KNIGHT_OFFSET); };
    uint64_t get_rook_occupancy() const { return get_pair_occupancy(ROOK_OFFSET); };
    uint64_t get_pawn_occupancy() const { return get_pair_occupancy(PAWN_OFFSET); };

    uint64_t get_king_occupancy(int side) const { return bitboards[KING_OFFSET + side]; };
    uint64_t get_queen_occupancy(int side) const { return bitboards[QUEEN_OFFSET + side]; };
    uint64_t get_bishop_occupancy(int side) const { return bitboards[BISHOP_OFFSET + side]; };
    uint64_t get_knight_occupancy(int side) const { return bitboards[KNIGHT_OFFSET + side]; };
    uint64_t get_rook_occupancy(int side) const { return bitboards[ROOK_OFFSET + side]; };
    uint64_t get_pawn_occupancy(int side) const { return bitboards[PAWN_OFFSET + side]; };

    int get_score(int side);

    void set_piece(uint_fast8_t piece, uint_fast8_t pos);
    void print_board() const;
    void clear_board();

    void set_from_fen(const char *input);

    void make_move(Move to_make);
};