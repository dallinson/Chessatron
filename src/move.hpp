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

enum class MoveFlags : uint_fast8_t {
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
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
        uint_fast16_t move = 0;

    public:
        Move() : move(0){};
        Move(uint_fast16_t v) : move(v){};
        Move(MoveFlags flags, uint_fast8_t dest, uint_fast8_t src) : move((((uint_fast16_t) flags) << 12) | (((uint_fast16_t) dest) << 6) | src){};
        static const Move NULL_MOVE;

        uint_fast16_t get_move() const { return move; };
        uint_fast8_t get_src_square() const { return GET_BITS(move, 5, 0); };
        uint_fast8_t get_dest_square() const { return GET_BITS(move, 11, 6); };

        uint_fast8_t get_src_rank() const { return GET_BITS(move, 5, 3); };
        uint_fast8_t get_src_file() const { return GET_BITS(move, 2, 0); };

        uint_fast8_t get_dest_rank() const { return GET_BITS(move, 11, 9); };
        uint_fast8_t get_dest_file() const { return GET_BITS(move, 8, 6); };

        MoveFlags get_move_flags() const { return (MoveFlags) GET_BITS(move, 15, 12); };

        bool is_null_move() const { return move == 0; };
        bool is_capture() const { return (static_cast<int>(get_move_flags()) & 0x04) != 0; };

        std::string to_string() const;
};

bool operator==(const Move& lhs, const Move& rhs);

class MoveList {
    private:
        size_t idx;
        std::array<Move, MAX_TURN_MOVE_COUNT> data;

    public:
        MoveList() : idx(0){};

        void add_move(const Move to_add) {
            this->data[idx] = to_add;
            idx += 1;
        };

        void add_moves(const MoveList& other_list) {
            size_t other_len = other_list.len();
            memcpy(&this->data[idx], other_list.get_data_addr(), other_len * sizeof(Move));
            idx += other_len;
        };

        Move& operator[](size_t arg_idx) { return data[arg_idx]; }
        const Move& operator[](size_t arg_idx) const { return data[arg_idx]; }

        const Move* get_data_addr() const { return data.data(); }

        size_t len() const { return this->idx; };
};

class PreviousMoveState {
    private:
        uint_fast32_t info;

    public:
        PreviousMoveState() : info(0){};
        PreviousMoveState(const Piece target_piece, const uint_fast8_t previous_en_passant_state, const bool white_kingside_castle,
                          const bool white_queenside_castle, const bool black_kingside_castle, const bool black_queenside_castle,
                          const uint8_t halfmove_clock)
            : info(target_piece.get_value() | previous_en_passant_state << 4 | white_kingside_castle << 8 | white_queenside_castle << 9 |
                   black_kingside_castle << 10 | black_queenside_castle << 11 | halfmove_clock << 12){};
        Piece get_piece() const { return GET_BITS(info, 3, 0); };
        uint_fast8_t get_previous_en_passant_file() const { return GET_BITS(info, 7, 4); };
        bool get_white_kingside_castle() const { return GET_BIT(info, 8); };
        bool get_white_queenside_castle() const { return GET_BIT(info, 9); };
        bool get_black_kingside_castle() const { return GET_BIT(info, 10); };
        bool get_black_queenside_castle() const { return GET_BIT(info, 11); };

        uint8_t get_halfmove_clock() const { return GET_BITS(info, 18, 12); };
};

class MoveHistory {
    private:
        size_t idx;
        std::pair<Move, PreviousMoveState> data[MAX_GAME_MOVE_COUNT];

    public:
        MoveHistory() : idx(0){};
        size_t len() { return idx; };

        void push_move(const std::pair<Move, PreviousMoveState>& to_add) {
            this->data[idx] = to_add;
            idx += 1;
        };

        std::pair<Move, PreviousMoveState> pop_move() {
            idx -= 1;
            return this->data[idx];
        };
};
