#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

#include "pieces.hpp"
#include "utils.hpp"

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

        uint16_t value() const { return move; };
        uint8_t src_sq() const { return get_bits(move, 5, 0); };
        uint8_t dst_sq() const { return get_bits(move, 11, 6); };

        uint8_t src_rnk() const { return get_bits(move, 5, 3); };
        uint8_t src_fle() const { return get_bits(move, 2, 0); };

        uint8_t dst_rnk() const { return get_bits(move, 11, 9); };
        uint8_t dst_fle() const { return get_bits(move, 8, 6); };

        MoveFlags get_move_flags() const { return (MoveFlags) get_bits(move, 15, 12); };
        PieceTypes get_promotion_piece_type() const { return static_cast<PieceTypes>((static_cast<int>(get_move_flags()) & 0b0011) + 2); };

        bool is_null_move() const { return move == 0; };
        bool is_capture() const { return (static_cast<int>(get_move_flags()) & 0x04) != 0; };
        bool is_promotion() const { return static_cast<int>(get_move_flags()) >= 8; };
        bool is_castling_move() const { return get_move_flags() == MoveFlags::QUEENSIDE_CASTLE || get_move_flags() == MoveFlags::KINGSIDE_CASTLE; };
        bool is_quiet() const { return !(is_capture() || is_promotion()); };

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
        std::array<ScoredMove, MAX_TURN_MOVE_COUNT> data;

    public:
        MoveList() : idx(0){};

        void add_move(const Move to_add) {
            this->data[idx].move = to_add;
            idx += 1;
        };

        void add_moves(const MoveList& other_list) {
            size_t other_len = other_list.size();
            memcpy(&this->data[idx], other_list.get_data_addr(), other_len * sizeof(Move));
            idx += other_len;
        };

        ScoredMove& operator[](size_t arg_idx) { return data[arg_idx]; }
        const ScoredMove& operator[](size_t arg_idx) const { return data[arg_idx]; }

        const ScoredMove* get_data_addr() const { return data.data(); }

        size_t size() const { return this->idx; };

        auto begin() const { return data.begin(); };
        auto begin() { return data.begin(); };
        auto end() const { return data.end(); };
        auto end() { return data.end(); };

        void clear() { this->idx = 0; };
};

