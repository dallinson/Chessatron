#pragma once

#include <array>
#include <cstdint>

#include "move.hpp"
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
        std::array<Bitboard, 12> bitboards = {0};

        std::array<Piece, 64> pieces = {0};

        uint_fast8_t en_passant_file = 9;

        // first 2 elems are kingside, second two queenside
        std::array<bool, 4> castling = {false, false, false, false};

        Side side_to_move = Side(0);

        inline Bitboard get_pair_occupancy(int offset) const { return bitboards[offset] | bitboards[offset + 1]; };

        std::array<Bitboard, 2> checkers = {0};
        std::array<Bitboard, 2> pinned_pieces = {0};

        ZobristKey zobrist_key;

    public:
        inline Bitboard get_occupancy() const {
            return get_king_occupancy() | get_queen_occupancy() | get_bishop_occupancy() | get_knight_occupancy() | get_rook_occupancy() |
                   get_pawn_occupancy();
        };
        inline Bitboard get_occupancy(const Side side) const { return get_side_occupancy(side); };

        inline Bitboard get_side_occupancy(const Side side) const {
            return bitboards[KING_OFFSET + static_cast<uint_fast8_t>(side)] | bitboards[QUEEN_OFFSET + static_cast<uint_fast8_t>(side)] |
                   bitboards[BISHOP_OFFSET + static_cast<uint_fast8_t>(side)] | bitboards[KNIGHT_OFFSET + static_cast<uint_fast8_t>(side)] |
                   bitboards[ROOK_OFFSET + static_cast<uint_fast8_t>(side)] | bitboards[PAWN_OFFSET + static_cast<uint_fast8_t>(side)];
        };
        inline Bitboard get_white_occupancy() const { return get_side_occupancy(Side::WHITE); };
        inline Bitboard get_black_occupancy() const { return get_side_occupancy(Side::WHITE); };

        inline Bitboard get_king_occupancy() const { return get_pair_occupancy(KING_OFFSET); };
        inline Bitboard get_queen_occupancy() const { return get_pair_occupancy(QUEEN_OFFSET); };
        inline Bitboard get_bishop_occupancy() const { return get_pair_occupancy(BISHOP_OFFSET); };
        inline Bitboard get_knight_occupancy() const { return get_pair_occupancy(KNIGHT_OFFSET); };
        inline Bitboard get_rook_occupancy() const { return get_pair_occupancy(ROOK_OFFSET); };
        inline Bitboard get_pawn_occupancy() const { return get_pair_occupancy(PAWN_OFFSET); };

        inline Bitboard get_king_occupancy(const Side side) const { return bitboards[KING_OFFSET + static_cast<uint_fast8_t>(side)]; };
        inline Bitboard get_queen_occupancy(const Side side) const { return bitboards[QUEEN_OFFSET + static_cast<uint_fast8_t>(side)]; };
        inline Bitboard get_bishop_occupancy(const Side side) const { return bitboards[BISHOP_OFFSET + static_cast<uint_fast8_t>(side)]; };
        inline Bitboard get_knight_occupancy(const Side side) const { return bitboards[KNIGHT_OFFSET + static_cast<uint_fast8_t>(side)]; };
        inline Bitboard get_rook_occupancy(const Side side) const { return bitboards[ROOK_OFFSET + static_cast<uint_fast8_t>(side)]; };
        inline Bitboard get_pawn_occupancy(const Side side) const { return bitboards[PAWN_OFFSET + static_cast<uint_fast8_t>(side)]; };

        /**
         * @brief Get the en passant file
         *
         * @return A uint_fast8_t of the en passant file from 0 to 7.  If en passant is not possible, its value is 9.
         */
        uint_fast8_t get_en_passant_file() const { return en_passant_file; };
        void set_en_passant_file(int file) { en_passant_file = file; };

        inline bool get_queenside_castling(const Side side) const { return castling[2 + side]; };
        inline bool get_kingside_castling(const Side side) const { return castling[side]; };
        inline void set_kingside_castling(const Side side, const bool val) { castling[side] = val; };
        inline void set_queenside_castling(const Side side, const bool val) { castling[2 + side] = val; };

        int get_score(Side side);

        void set_piece(uint_fast8_t piece, uint_fast8_t pos);
        Piece get_piece(const int i) const { return this->pieces[i]; };
        void print_board() const;
        void clear_board();

        bool set_from_fen(const char* input);

        Side get_side_to_move() const { return this->side_to_move; };

        void make_move(const Move to_make, MoveHistory& move_history);
        void unmake_move(MoveHistory& move_history);

        void recompute_blockers_and_checkers() {
            recompute_blockers_and_checkers(Side::WHITE);
            recompute_blockers_and_checkers(Side::BLACK);
        };
        void recompute_blockers_and_checkers(const Side side);
        inline Bitboard get_checkers(const Side side) const { return checkers[static_cast<int>(side)]; };
        inline Bitboard get_pinned_pieces(const Side side) const { return pinned_pieces[static_cast<int>(side)]; };

        inline ZobristKey get_zobrist_key() const { return zobrist_key; };
};

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs);