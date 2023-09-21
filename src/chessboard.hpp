#pragma once

#include <array>
#include <cstdint>

#include "pieces.hpp"
#include "utils.hpp"
#include "move.hpp"

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
    std::array<Bitboard, 12> bitboards = {0};

    std::array<Piece, 64> pieces = {0};

    uint_fast8_t en_passant_file = 9;

    std::array<bool, 2> kingside_castling = {false, false};
    std::array<bool, 2> queenside_castling = {false, false};

    uint_fast8_t side = 0;

    inline Bitboard get_pair_occupancy(int offset) const { return bitboards[offset] | bitboards[offset + 1]; };


  public:
    Bitboard get_occupancy() const { return get_king_occupancy() | get_queen_occupancy() | get_bishop_occupancy() | get_knight_occupancy() | get_rook_occupancy() | get_pawn_occupancy(); };
    Bitboard get_occupancy(const int side) const { return get_side_occupancy(side); };

    Bitboard get_side_occupancy(const int side) const { return bitboards[KING_OFFSET + side] | bitboards[QUEEN_OFFSET + side] | bitboards[BISHOP_OFFSET + side] | bitboards[KNIGHT_OFFSET + side] | bitboards[ROOK_OFFSET + side] | bitboards[PAWN_OFFSET + side]; };
    Bitboard get_white_occupancy() const { return get_side_occupancy(WHITE_IDX); };
    Bitboard get_black_occupancy() const { return get_side_occupancy(BLACK_IDX); };

    Bitboard get_king_occupancy() const { return get_pair_occupancy(KING_OFFSET); };
    Bitboard get_queen_occupancy() const { return get_pair_occupancy(QUEEN_OFFSET); };
    Bitboard get_bishop_occupancy() const { return get_pair_occupancy(BISHOP_OFFSET); };
    Bitboard get_knight_occupancy() const { return get_pair_occupancy(KNIGHT_OFFSET); };
    Bitboard get_rook_occupancy() const { return get_pair_occupancy(ROOK_OFFSET); };
    Bitboard get_pawn_occupancy() const { return get_pair_occupancy(PAWN_OFFSET); };

    Bitboard get_king_occupancy(const int side) const { return bitboards[KING_OFFSET + side]; };
    Bitboard get_queen_occupancy(const int side) const { return bitboards[QUEEN_OFFSET + side]; };
    Bitboard get_bishop_occupancy(const int side) const { return bitboards[BISHOP_OFFSET + side]; };
    Bitboard get_knight_occupancy(const int side) const { return bitboards[KNIGHT_OFFSET + side]; };
    Bitboard get_rook_occupancy(const int side) const { return bitboards[ROOK_OFFSET + side]; };
    Bitboard get_pawn_occupancy(const int side) const { return bitboards[PAWN_OFFSET + side]; };

    /**
     * @brief Get the en passant file 
     * 
     * @return A uint_fast8_t of the en passant file from 0 to 7.  If en passant is not possible, its value is 9.
     */
    uint_fast8_t get_en_passant_file() const { return en_passant_file; };
    void set_en_passant_file(int file) { en_passant_file = file; };

    bool get_queenside_castling(const int side) const { return queenside_castling[side]; };
    bool get_kingside_castling(const int side) const { return kingside_castling[side]; };
    void set_kingside_castling(const int side, const bool val) { kingside_castling[side] = val; };
    void set_queenside_castling(const int side, const bool val) { queenside_castling[side] = val; };

    int get_score(int side);

    void set_piece(uint_fast8_t piece, uint_fast8_t pos);
    Piece get_piece(const int i) const { return this->pieces[i]; };
    void print_board() const;
    void clear_board();

    bool set_from_fen(const char *input);

    uint_fast8_t get_side() { return this->side; };

    void make_move(const Move to_make, MoveHistory& move_history);
    void unmake_move(MoveHistory& move_history);
};

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs);