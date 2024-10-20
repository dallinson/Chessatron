#pragma once

#include <cassert>
#include <array>
#include <cstdint>
#include <optional>

#include "bitboard.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "utils.hpp"
#include "zobrist_hashing.hpp"

template <PieceTypes p> uint8_t bb_idx = static_cast<int>(p) - 1;

class BoardHistory;

class Position {
    private:
        std::array<Bitboard, 6> piece_bbs = {0};
        std::array<Bitboard, 2> side_bbs = {0};

        std::array<Piece, 64> piece_mb = {0};

        uint8_t en_passant_file = 9; 

        // first 2 elems are kingside, second two queenside
        uint8_t castling = 0;

        Side side_to_move = Side(0);


        Bitboard _checkers = 0;
        Bitboard _pinned_pieces = 0;

        std::array<int32_t, 2> scores = {0};
        uint8_t mg_phase = 0;

        ZobristKey _zobrist_key = 0;
        ZobristKey _pawn_hash = 0;
        int halfmove_clock = 0;
        int fullmove_counter = 0;

    public:
        Position() = default;
        Position(const Position& origin, const Move to_make);
        Position& make_move(const Move to_make, BoardHistory& move_history) const;

        inline Bitboard occupancy() const {
            return side_bbs[0] | side_bbs[1];
        };

        inline Bitboard occupancy(const Side side) const {
            return side_bbs[static_cast<int>(side)];
        };

        inline Bitboard kings() const { return piece_bbs[bb_idx<PieceTypes::KING>]; };
        inline Bitboard queens() const { return piece_bbs[bb_idx<PieceTypes::QUEEN>]; };
        inline Bitboard bishops() const { return piece_bbs[bb_idx<PieceTypes::BISHOP>]; };
        inline Bitboard knights() const { return piece_bbs[bb_idx<PieceTypes::KNIGHT>]; };
        inline Bitboard rooks() const { return piece_bbs[bb_idx<PieceTypes::ROOK>]; };
        inline Bitboard pawns() const { return piece_bbs[bb_idx<PieceTypes::PAWN>]; };
        inline Bitboard get_bb(const int piece_type, const int side) const {
            return piece_bbs[piece_type] & side_bbs[side];
        }
        template <PieceTypes piece_type> inline Bitboard pieces() const { return piece_bbs[bb_idx<piece_type>()]; };
        template <PieceTypes piece_type> inline Bitboard pieces(const Side side) const { return piece_bbs[bb_idx<piece_type>] & side_bbs[static_cast<int>(side)]; };

        inline Bitboard kings(const Side side) const {
            return piece_bbs[bb_idx<PieceTypes::KING>] & side_bbs[static_cast<uint8_t>(side)];
        };
        inline Bitboard queens(const Side side) const {
            return piece_bbs[bb_idx<PieceTypes::QUEEN>] & side_bbs[static_cast<uint8_t>(side)];
        };
        inline Bitboard bishops(const Side side) const {
            return piece_bbs[bb_idx<PieceTypes::BISHOP>] & side_bbs[static_cast<uint8_t>(side)];
        };
        inline Bitboard knights(const Side side) const {
            return piece_bbs[bb_idx<PieceTypes::KNIGHT>] & side_bbs[static_cast<uint8_t>(side)];
        };
        inline Bitboard rooks(const Side side) const {
            return piece_bbs[bb_idx<PieceTypes::ROOK>] & side_bbs[static_cast<uint8_t>(side)];
        };
        inline Bitboard pawns(const Side side) const {
            return piece_bbs[bb_idx<PieceTypes::PAWN>] & side_bbs[static_cast<uint8_t>(side)];
        };

        inline Piece piece_at(const Square sq) const {
            return piece_mb[sq_to_int(sq)];
        }

        /**
         * @brief Get the en passant file
         *
         * @return A uint8_t of the en passant file from 0 to 7.  If en passant is not possible, its value is 9.
         */
        uint8_t get_en_passant_file() const { return en_passant_file; };
        void set_en_passant_file(int file) {
            _zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
            en_passant_file = file;
            _zobrist_key ^= ZobristKeys::EnPassantKeys[file];
        };

        inline bool get_queenside_castling(const Side side) const { return get_bit(castling, 2 + static_cast<uint8_t>(side)); };
        inline bool get_kingside_castling(const Side side) const { return get_bit(castling, static_cast<uint8_t>(side)); };
        inline uint8_t get_castling() const { return castling; };
        inline void set_kingside_castling(const Side side, const bool val) {
            const int offset = static_cast<int>(side);
            if (get_bit(castling, offset) != val) {
                _zobrist_key ^= ZobristKeys::CastlingKeys[offset];
                toggle_bit(castling, offset);
            }
        };
        inline void set_queenside_castling(const Side side, const bool val) {
            const int offset = 2 + static_cast<int>(side);
            if (get_bit(castling, offset) != val) {
                _zobrist_key ^= ZobristKeys::CastlingKeys[offset];
                toggle_bit(castling, offset);
            }
        };

        void set_piece(Piece piece, Square sq);
        void print_board() const;
        void clear_board();

        std::optional<int> set_from_fen(const std::string input);

        Side stm() const { return this->side_to_move; };

        int get_fullmove_counter() const { return this->fullmove_counter; };
        int get_halfmove_clock() const { return this->halfmove_clock; };

        void recompute_blockers_and_checkers(const Side side);

        int piece_to(Move move) const { return piece_at(move.src_sq()).get_value() << 6 | sq_to_int(move.dst_sq()); };

        inline Bitboard checkers() const { return _checkers; };
        inline Bitboard pinned_pieces() const { return _pinned_pieces; };
        bool in_check() const { return !_checkers.empty(); }; 

        int32_t get_score(Side side) const { return scores[static_cast<int>(side)]; };
        uint8_t get_mg_phase() const { return mg_phase; };

        inline ZobristKey zobrist_key() const { return _zobrist_key; };
        ZobristKey pawn_hash() const { return _pawn_hash; };
        ZobristKey get_polyglot_zobrist_key() const {
            auto default_key = this->_zobrist_key;
            if (en_passant_file != 9) {
                const Side enemy = enemy_side(side_to_move);
                const size_t offset = en_passant_file + 8 * static_cast<int>(enemy);
                if ((pawns(side_to_move) & ZobristKeys::EnPassantCheckBitboards[offset]).empty()) {
                    default_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
                }
            }
            return default_key;
        }
        ZobristKey key_after(const Move move) const;

        std::optional<Move> generate_move_from_string(const std::string& m) const;
};

bool operator==(const Position& lhs, const Position& rhs);

class BoardHistory {
    private:
        std::vector<Position> board_hist;
        // The move at index i is the move made to reach the board at index i, or
        // alternatively the move made at index i - 1
        std::array<Move, MAX_GAME_MOVE_COUNT> move_hist;
        size_t idx;

    public:
        BoardHistory() : idx(0) {
            board_hist.resize(MAX_GAME_MOVE_COUNT);
        };
        BoardHistory(const Position& board) {
            idx = 0;
            board_hist.resize(MAX_GAME_MOVE_COUNT);
            push_board(board);
        }

        Position& push_board(const Position new_board, const Move move = Move::NULL_MOVE()) {
            this->board_hist[idx] = new_board;
            this->move_hist[idx] = move;
            idx += 1;
            return this->board_hist[idx - 1];
        }

        Position& pop_board() {
            assert(idx >= 2);
            idx -= 1;
            return this->board_hist[idx - 1];
        }

        size_t len() const { return idx; };
        const Position& operator[](size_t idx) const { return board_hist[idx]; };
        Position& operator[](size_t idx) { return board_hist[idx]; };
        Move move_at(size_t idx) const { return move_hist[idx]; };
        size_t conthist_idx(size_t idx) const {
            const auto move = move_hist[idx];
            return (board_hist[idx - 1].piece_at(move.src_sq()).get_value() << 6) | sq_to_int(move.dst_sq());
        };

        void clear() { idx = 0; };
};