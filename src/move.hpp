#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

#include "pieces.hpp"
#include "utils.hpp"

#define MAX_TURN_MOVE_COUNT 218
#define MAX_GAME_MOVE_COUNT 5899
// the maximum possible number of moves, currently 218 on position R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1

enum class MoveFlags : uint8_t {
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
    KINGSIDE_CASTLE = 2,
    QUEENSIDE_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT_CAPTURE = 5,
    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION = 9,
    ROOK_PROMOTION = 10,
    QUEEN_PROMOTION = 11,
    KNIGHT_PROMOTION_CAPTURE = 12,
    BISHOP_PROMOTION_CAPTURE = 13,
    ROOK_PROMOTION_CAPTURE = 14,
    QUEEN_PROMOTION_CAPTURE = 15,
};

class Move {
    private:
        uint16_t move;

    public:
        Move(){};
        Move(uint16_t v) : move(v){};
        Move(MoveFlags flags, uint_fast8_t dest, uint_fast8_t src) : move((((uint16_t) flags) << 12) | (((uint16_t) dest) << 6) | src){};
        static const Move NULL_MOVE;

        uint16_t get_move() const { return move; };
        uint8_t get_src_square() const { return get_bits(move, 5, 0); };
        uint8_t get_dest_square() const { return get_bits(move, 11, 6); };

        uint8_t get_src_rank() const { return get_bits(move, 5, 3); };
        uint8_t get_src_file() const { return get_bits(move, 2, 0); };

        uint8_t get_dest_rank() const { return get_bits(move, 11, 9); };
        uint8_t get_dest_file() const { return get_bits(move, 8, 6); };

        MoveFlags get_move_flags() const { return (MoveFlags) get_bits(move, 15, 12); };
        PieceTypes get_promotion_piece_type() const { return static_cast<PieceTypes>((static_cast<int>(get_move_flags()) & 0b0011) + 2); };

        bool is_null_move() const { return move == 0; };
        bool is_capture() const { return (static_cast<int>(get_move_flags()) & 0x04) != 0; };
        bool is_promotion() const { return static_cast<int>(get_move_flags()) >= 8; };
        bool is_castling_move() const { return get_move_flags() == MoveFlags::QUEENSIDE_CASTLE || get_move_flags() == MoveFlags::KINGSIDE_CASTLE; };

        uint16_t get_history_idx(Side side_to_move) const { return (static_cast<int>(side_to_move) << 12) + get_bits(move, 11, 0); };

        std::string to_string() const;
};

struct ScoredMove {
    int32_t score;
    Move move;
    bool see_ordering_result;
    uint8_t padding;
};

bool operator==(const Move& lhs, const Move& rhs);

class MoveList {
    private:
        size_t idx;
        ScoredMove data[MAX_TURN_MOVE_COUNT];

    public:
        MoveList() : idx(0){};

        void add_move(const Move to_add) {
            this->data[idx].move = to_add;
            idx += 1;
        };

        void add_moves(const MoveList& other_list) {
            size_t other_len = other_list.len();
            memcpy(&this->data[idx], other_list.get_data_addr(), other_len * sizeof(Move));
            idx += other_len;
        };

        ScoredMove& operator[](size_t arg_idx) { return data[arg_idx]; }
        const ScoredMove& operator[](size_t arg_idx) const { return data[arg_idx]; }

        const ScoredMove* get_data_addr() const { return data; }

        size_t len() const { return this->idx; };
};

class MoveHistoryEntry {
    private:
        ZobristKey z;
        // 8 bytes
        Move m;
        // 2 bytes
        Piece p;
        // 1 byte
        uint8_t castling;
        // 1 byte
        uint8_t previous_en_passant;
        // 1 byte
        uint8_t previous_halfmove_clock;
        // 1 byte
        uint16_t padding;
        // pad to 16 bytes; 64 bytes (cache line size) is easily divisible by 16

    public:
        MoveHistoryEntry(){};
        MoveHistoryEntry(const ZobristKey key, const Move move, const Piece target_piece, const uint8_t en_passant, uint8_t previous_halfmove_clock,
                         const bool white_kingside_castle, const bool black_kingside_castle, const bool white_queenside_castle,
                         const bool black_queenside_castle)
            : z(key), m(move), p(target_piece),
              castling(white_kingside_castle << 3 | black_kingside_castle << 2 | white_queenside_castle << 1 | black_queenside_castle),
              previous_en_passant(en_passant), previous_halfmove_clock(previous_halfmove_clock){};
        Piece get_piece() const { return p; };
        Move get_move() const { return m; };
        uint_fast8_t get_previous_en_passant_file() const { return previous_en_passant; };
        bool get_white_kingside_castle() const { return get_bit(castling, 3); };
        bool get_white_queenside_castle() const { return get_bit(castling, 1); };
        bool get_black_kingside_castle() const { return get_bit(castling, 2); };
        bool get_black_queenside_castle() const { return get_bit(castling, 0); };

        uint8_t get_halfmove_clock() const { return previous_halfmove_clock; };

        ZobristKey get_zobrist_key() const { return z; };
};

class MoveHistory {
    private:
        size_t idx;
        MoveHistoryEntry data[MAX_GAME_MOVE_COUNT];

    public:
        MoveHistory() : idx(0){};
        size_t len() const { return idx; };

        void push_move(const MoveHistoryEntry& to_add) {
            this->data[idx] = to_add;
            idx += 1;
        };

        MoveHistoryEntry pop_move() {
            idx -= 1;
            return this->data[idx];
        };

        const MoveHistoryEntry& operator[](size_t idx) const { return data[idx]; }
};
