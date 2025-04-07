#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <fmt/format.h>
#include <string>

#include "pieces.hpp"
#include "uci_options.hpp"
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

inline MoveFlags operator|(const MoveFlags lhs, const MoveFlags rhs) {
    return static_cast<MoveFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

class Move {
    private:
        uint16_t move;

    public:
        constexpr Move(){};
        constexpr Move(uint16_t v) : move(v){};
        constexpr Move(MoveFlags flags, uint_fast8_t dest, uint_fast8_t src) : move((((uint16_t) flags) << 12) | (((uint16_t) dest) << 6) | src){};
        constexpr Move(MoveFlags flags, Square dest, Square src) : Move(flags, sq_to_int(dest), sq_to_int(src)) {};
        constexpr static Move NULL_MOVE() { return Move(0); };

        constexpr uint16_t value() const { return move; };
        constexpr Square src_sq() const { return static_cast<Square>(get_bits(move, 5, 0)); };
        constexpr Square dst_sq() const { return static_cast<Square>(get_bits(move, 11, 6)); };

        constexpr uint8_t src_rnk() const { return get_bits(move, 5, 3); };
        constexpr uint8_t src_fle() const { return get_bits(move, 2, 0); };

        constexpr uint8_t dst_rnk() const { return get_bits(move, 11, 9); };
        constexpr uint8_t dst_fle() const { return get_bits(move, 8, 6); };

        constexpr MoveFlags flags() const { return (MoveFlags) get_bits(move, 15, 12); };
        constexpr PieceTypes promo_type() const { return static_cast<PieceTypes>((static_cast<int>(flags()) & 0b0011) + 2); };

        constexpr bool is_null_move() const { return move == 0; };
        constexpr bool is_capture() const { return (static_cast<int>(flags()) & 0x04) != 0; };
        constexpr bool is_promotion() const { return static_cast<int>(flags()) >= 8; };
        constexpr bool is_castling() const { return flags() == MoveFlags::QUEENSIDE_CASTLE || flags() == MoveFlags::KINGSIDE_CASTLE; };
        constexpr bool is_quiet() const { return !(is_capture() || is_promotion() || is_castling()); };
        constexpr bool is_noisy() const { return !is_quiet(); };

        constexpr uint16_t hist_idx(Side stm) const { return (static_cast<int>(stm) << 12) + get_bits(move, 11, 0); };
};

template <>
struct fmt::formatter<Move> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Move& _move, fmt::format_context& ctx) const {
        const auto move = [&]() {
            if ((!_move.is_castling()) || (static_cast<bool>(uci_options()["UCI_Chess960"]))) {
                // if this isn't castling, or dfrc is enabled
                return _move;
            } else {
                auto rnk = _move.dst_rnk();
                auto dst_fle = _move.flags() == MoveFlags::KINGSIDE_CASTLE ? 6 : 2;
                const auto dst = square(rnk, dst_fle);
                return Move(_move.flags(), dst, _move.src_sq());
            }
        }();
        if (move.src_sq() == Square::A1 && move.dst_sq() == Square::A1) {
            // if this is a null move
            return fmt::format_to(ctx.out(), "{}", "0000");
        }
        std::string to_return;
        to_return.push_back(move.src_fle() + 'a');
        to_return.push_back(move.src_rnk() + '1');
    
        to_return.push_back(move.dst_fle() + 'a');
        to_return.push_back(move.dst_rnk() + '1');
    
        if (move.is_promotion()) {
            switch (move.promo_type()) {
            case PieceTypes::ROOK:
                to_return.push_back('r');
                break;
            case PieceTypes::KNIGHT:
                to_return.push_back('n');
                break;
            case PieceTypes::BISHOP:
                to_return.push_back('b');
                break;
            case PieceTypes::QUEEN:
                to_return.push_back('q');
                break;
            default:
                break;
            }
        }
        
        return fmt::format_to(ctx.out(), "{}", to_return);
    }
};

struct ScoredMove {
    int32_t score;
    Move move;
    bool see_ordering_result;
    uint8_t padding;

    ScoredMove() {};
    ScoredMove(Move move) : move(move) {};
};

bool operator==(const Move& lhs, const Move& rhs);

template<typename T, size_t elem_count>
class StackVector {
    private:
        size_t idx;
        std::array<T, elem_count> data;

    public:
        StackVector() : idx(0) {};

        void add(const T to_add) {
            this->data[idx] = to_add;
            idx += 1;
        };

        T& operator[](size_t arg_idx) { return data[arg_idx]; }
        const T& operator[](size_t arg_idx) const { return data[arg_idx]; }

        size_t size() const { return this->idx; };

        auto begin() const { return data.begin(); };
        auto begin() { return data.begin(); };
        auto end() const { return data.begin() + idx; };
        auto end() { return data.begin() + idx; };

        void clear() { this->idx = 0; };
};

using MoveList = StackVector<ScoredMove, MAX_TURN_MOVE_COUNT>;
using UnscoredMoveList = StackVector<Move, MAX_TURN_MOVE_COUNT>;