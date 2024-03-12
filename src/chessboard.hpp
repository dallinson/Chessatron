#pragma once

#include <cassert>
#include <array>
#include <cstdint>
#include <optional>

#include "move.hpp"
#include "pieces.hpp"
#include "utils.hpp"
#include "zobrist_hashing.hpp"

template <PieceTypes p> uint8_t bb_idx = 2 * (static_cast<int>(p) - 1);

class BoardHistory;

class ChessBoard {
    private:
        std::array<Bitboard, 12> bbs = {0};

        std::array<Piece, 64> pieces = {0};

        uint8_t en_passant_file = 9; 

        // first 2 elems are kingside, second two queenside
        uint8_t castling = 0;

        Side side_to_move = Side(0);

        inline Bitboard get_pair_occupancy(int offset) const { return bbs[offset] | bbs[offset + 1]; };

        Bitboard checkers = 0;
        Bitboard pinned_pieces = 0;

        std::array<int32_t, 2> scores = {0};
        uint8_t mg_phase = 0;

        ZobristKey zobrist_key = 0;
        int halfmove_clock = 0;
        int fullmove_counter = 0;

    public:
        ChessBoard() = default;
        ChessBoard(const ChessBoard& origin, const Move to_make);
        ChessBoard& make_move(const Move to_make, BoardHistory& move_history) const;

        inline Bitboard occupancy() const {
            return kings() | queens() | bishops() | knights() | rooks() |
                   pawns();
        };

        inline Bitboard occupancy(const Side side) const {
            return bbs[bb_idx<PieceTypes::KING> + static_cast<uint8_t>(side)] |
                   bbs[bb_idx<PieceTypes::QUEEN> + static_cast<uint8_t>(side)] |
                   bbs[bb_idx<PieceTypes::BISHOP> + static_cast<uint8_t>(side)] |
                   bbs[bb_idx<PieceTypes::KNIGHT> + static_cast<uint8_t>(side)] |
                   bbs[bb_idx<PieceTypes::ROOK> + static_cast<uint8_t>(side)] |
                   bbs[bb_idx<PieceTypes::PAWN> + static_cast<uint8_t>(side)];
        };

        inline Bitboard kings() const { return get_pair_occupancy(bb_idx<PieceTypes::KING>); };
        inline Bitboard queens() const { return get_pair_occupancy(bb_idx<PieceTypes::QUEEN>); };
        inline Bitboard bishops() const { return get_pair_occupancy(bb_idx<PieceTypes::BISHOP>); };
        inline Bitboard knights() const { return get_pair_occupancy(bb_idx<PieceTypes::KNIGHT>); };
        inline Bitboard rooks() const { return get_pair_occupancy(bb_idx<PieceTypes::ROOK>); };
        inline Bitboard pawns() const { return get_pair_occupancy(bb_idx<PieceTypes::PAWN>); };
        template <PieceTypes piece_type> inline Bitboard get_pieces() const { return get_pair_occupancy(bb_idx<piece_type>()); };

        inline Bitboard kings(const Side side) const {
            return bbs[bb_idx<PieceTypes::KING> + static_cast<uint8_t>(side)];
        };
        inline Bitboard queens(const Side side) const {
            return bbs[bb_idx<PieceTypes::QUEEN> + static_cast<uint8_t>(side)];
        };
        inline Bitboard bishops(const Side side) const {
            return bbs[bb_idx<PieceTypes::BISHOP> + static_cast<uint8_t>(side)];
        };
        inline Bitboard knights(const Side side) const {
            return bbs[bb_idx<PieceTypes::KNIGHT> + static_cast<uint8_t>(side)];
        };
        inline Bitboard rooks(const Side side) const {
            return bbs[bb_idx<PieceTypes::ROOK> + static_cast<uint8_t>(side)];
        };
        inline Bitboard get_pawns(const Side side) const {
            return bbs[bb_idx<PieceTypes::PAWN> + static_cast<uint8_t>(side)];
        };
        template <PieceTypes piece_type> inline Bitboard occupancy_of(const Side side) const {
            return bbs[(2 * ((static_cast<int>(piece_type)) - 1)) + static_cast<uint8_t>(side)];
        };

        inline Bitboard get_bb(const int idx) const { return bbs[idx]; }

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

        inline bool get_queenside_castling(const Side side) const { return get_bit(castling, 2 + static_cast<uint8_t>(side)); };
        inline bool get_kingside_castling(const Side side) const { return get_bit(castling, static_cast<uint8_t>(side)); };
        inline uint8_t get_castling() const { return castling; };
        inline void set_kingside_castling(const Side side, const bool val) {
            const int offset = static_cast<int>(side);
            if (get_bit(castling, offset) != val) {
                zobrist_key ^= ZobristKeys::CastlingKeys[offset];
                toggle_bit(castling, offset);
            }
        };
        inline void set_queenside_castling(const Side side, const bool val) {
            const int offset = 2 + static_cast<int>(side);
            if (get_bit(castling, offset) != val) {
                zobrist_key ^= ZobristKeys::CastlingKeys[offset];
                toggle_bit(castling, offset);
            }
        };

        void set_piece(Piece piece, uint8_t pos);
        Piece get_piece(const int i) const { return this->pieces[i]; };
        void print_board() const;
        void clear_board();

        std::optional<int> set_from_fen(const std::string input);

        Side get_side_to_move() const { return this->side_to_move; };


        int get_fullmove_counter() const { return this->fullmove_counter; };
        int get_halfmove_clock() const { return this->halfmove_clock; };

        void recompute_blockers_and_checkers(const Side side);

        inline Bitboard get_checkers() const { return checkers; };
        inline Bitboard get_pinned_pieces() const { return pinned_pieces; };
        bool in_check() const { return checkers != 0; }; 

        int32_t get_score(Side side) const { return scores[static_cast<int>(side)]; };
        uint8_t get_mg_phase() const { return mg_phase; };

        inline ZobristKey get_zobrist_key() const { return zobrist_key; };
        ZobristKey get_polyglot_zobrist_key() const {
            auto default_key = this->zobrist_key;
            if (en_passant_file != 9) {
                const Side enemy = enemy_side(side_to_move);
                const size_t offset = en_passant_file + 8 * static_cast<int>(enemy);
                if ((get_pawns(side_to_move) & ZobristKeys::EnPassantCheckBitboards[offset]) == 0) {
                    default_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
                }
            }
            return default_key;
        }

        std::optional<Move> generate_move_from_string(const std::string& m) const;
};

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs);

class BoardHistory {
    private:
        std::array<ChessBoard, MAX_GAME_MOVE_COUNT> data;
        size_t idx;

    public:
        BoardHistory() : idx(0) {};
        BoardHistory(const ChessBoard& board) {
            idx = 0;
            push_board(board);
        }

        ChessBoard& push_board(const ChessBoard new_board) {
            this->data[idx] = new_board;
            idx += 1;
            return this->data[idx - 1];
        }

        ChessBoard& pop_board() {
            assert(idx >= 2);
            idx -= 1;
            return this->data[idx - 1];
        }

        size_t len() const { return idx; };
        const ChessBoard& operator[](size_t idx) const { return data[idx]; };
        ChessBoard& operator[](size_t idx) { return data[idx]; };
};