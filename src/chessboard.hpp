#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "move.hpp"
#include "pieces.hpp"
#include "utils.hpp"
#include "zobrist_hashing.hpp"

template <PieceTypes p> uint8_t bitboard_offset = 2 * (static_cast<int>(p) - 1);

class ChessBoard {
    private:
        std::array<Bitboard, 12> bitboards = {0};

        std::array<Piece, 64> pieces = {0};

        uint8_t en_passant_file = 9;

        // first 2 elems are kingside, second two queenside
        std::array<bool, 4> castling = {false, false, false, false};

        Side side_to_move = Side(0);

        inline Bitboard get_pair_occupancy(int offset) const { return bitboards[offset] | bitboards[offset + 1]; };

        std::array<Bitboard, 2> checkers = {0};
        std::array<Bitboard, 2> pinned_pieces = {0};

        std::array<Score, 2> midgame_scores = {0};
        std::array<Score, 2> endgame_scores = {0};
        uint8_t midgame_phase = 0;

        ZobristKey zobrist_key = 0;
        int halfmove_clock = 0;
        int fullmove_counter = 0;

    public:
        inline Bitboard get_occupancy() const {
            return get_king_occupancy() | get_queen_occupancy() | get_bishop_occupancy() | get_knight_occupancy() | get_rook_occupancy() |
                   get_pawn_occupancy();
        };
        inline Bitboard get_occupancy(const Side side) const { return get_side_occupancy(side); };

        inline Bitboard get_side_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::KING> + static_cast<uint8_t>(side)] |
                   bitboards[bitboard_offset<PieceTypes::QUEEN> + static_cast<uint8_t>(side)] |
                   bitboards[bitboard_offset<PieceTypes::BISHOP> + static_cast<uint8_t>(side)] |
                   bitboards[bitboard_offset<PieceTypes::KNIGHT> + static_cast<uint8_t>(side)] |
                   bitboards[bitboard_offset<PieceTypes::ROOK> + static_cast<uint8_t>(side)] |
                   bitboards[bitboard_offset<PieceTypes::PAWN> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_white_occupancy() const { return get_side_occupancy(Side::WHITE); };
        inline Bitboard get_black_occupancy() const { return get_side_occupancy(Side::WHITE); };

        inline Bitboard get_king_occupancy() const { return get_pair_occupancy(bitboard_offset<PieceTypes::KING>); };
        inline Bitboard get_queen_occupancy() const { return get_pair_occupancy(bitboard_offset<PieceTypes::QUEEN>); };
        inline Bitboard get_bishop_occupancy() const { return get_pair_occupancy(bitboard_offset<PieceTypes::BISHOP>); };
        inline Bitboard get_knight_occupancy() const { return get_pair_occupancy(bitboard_offset<PieceTypes::KNIGHT>); };
        inline Bitboard get_rook_occupancy() const { return get_pair_occupancy(bitboard_offset<PieceTypes::ROOK>); };
        inline Bitboard get_pawn_occupancy() const { return get_pair_occupancy(bitboard_offset<PieceTypes::PAWN>); };
        template <PieceTypes piece_type> inline Bitboard get_piece_occupancy() const { return get_pair_occupancy(bitboard_offset<piece_type>()); };

        inline Bitboard get_king_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::KING> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_queen_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::QUEEN> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_bishop_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::BISHOP> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_knight_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::KNIGHT> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_rook_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::ROOK> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_pawn_occupancy(const Side side) const {
            return bitboards[bitboard_offset<PieceTypes::PAWN> + static_cast<uint8_t>(side)];
        };
        template <PieceTypes piece_type> inline Bitboard get_piece_occupancy(const Side side) const {
            return bitboards[(2 * ((static_cast<int>(piece_type)) - 1)) + static_cast<uint8_t>(side)];
        };

        inline Bitboard get_bitboard(const int idx) const { return bitboards.at(idx); }

        /**
         * @brief Get the en passant file
         *
         * @return A uint8_t of the en passant file from 0 to 7.  If en passant is not possible, its value is 9.
         */
        uint8_t get_en_passant_file() const { return en_passant_file; };
        void set_en_passant_file(int file) {
            zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
            en_passant_file = file;
            zobrist_key ^= ZobristKeys::EnPassantKeys[file];
        };

        inline bool get_queenside_castling(const Side side) const { return castling[2 + static_cast<int>(side)]; };
        inline bool get_kingside_castling(const Side side) const { return castling[static_cast<int>(side)]; };
        inline void set_kingside_castling(const Side side, const bool val) {
            const int offset = static_cast<int>(side);
            if (castling[offset] != val) {
                zobrist_key ^= ZobristKeys::CastlingKeys[offset];
            }
            castling[offset] = val;
        };
        inline void set_queenside_castling(const Side side, const bool val) {
            const int offset = 2 + static_cast<int>(side);
            if (castling[offset] != val) {
                zobrist_key ^= ZobristKeys::CastlingKeys[offset];
            }
            castling[offset] = val;
        };

        void set_piece(Piece piece, uint8_t pos);
        Piece get_piece(const int i) const { return this->pieces[i]; };
        void print_board() const;
        void clear_board();

        std::optional<int> set_from_fen(const std::string input);

        Side get_side_to_move() const { return this->side_to_move; };

        void make_move(const Move to_make, MoveHistory& move_history);
        void unmake_move(MoveHistory& move_history);

        int get_fullmove_counter() const { return this->fullmove_counter; };
        int get_halfmove_clock() const { return this->halfmove_clock; };

        void recompute_blockers_and_checkers() {
            recompute_blockers_and_checkers(Side::WHITE);
            recompute_blockers_and_checkers(Side::BLACK);
        };
        void recompute_blockers_and_checkers(const Side side);

        inline Bitboard get_checkers(const Side side) const { return checkers[static_cast<int>(side)]; };
        inline Bitboard get_pinned_pieces(const Side side) const { return pinned_pieces[static_cast<int>(side)]; };
        bool in_check(const Side side) const { return get_checkers(side) != 0; };
        bool in_check() const { return in_check(get_side_to_move()); }; 

        Score get_midgame_score(Side side) const { return midgame_scores[static_cast<int>(side)]; };
        Score get_endgame_score(Side side) const { return endgame_scores[static_cast<int>(side)]; };
        uint8_t get_midgame_phase() const { return midgame_phase; };

        inline ZobristKey get_zobrist_key() const { return zobrist_key; };
        ZobristKey get_polyglot_zobrist_key() const {
            auto default_key = this->zobrist_key;
            if (en_passant_file != 9) {
                const Side enemy_side = ENEMY_SIDE(side_to_move);
                const size_t offset = en_passant_file + 8 * static_cast<int>(enemy_side);
                if ((get_pawn_occupancy(side_to_move) & ZobristKeys::EnPassantCheckBitboards[offset]) == 0) {
                    default_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
                }
            }
            return default_key;
        }

        std::optional<Move> generate_move_from_string(const std::string& m) const;
};

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs);
