#pragma once

#include "utils.hpp"

#define SIDE_MASK BIT(3)
#define PIECE_MASK BITS(2, 0)

#define BLACK_PIECE BIT(3)

#define WHITE_IDX 0
#define BLACK_IDX 1

enum class PieceTypes : uint8_t { PAWN = 1, ROOK = 2, KNIGHT = 3, BISHOP = 4, QUEEN = 5, KING = 6 };

class Piece {
    private:
        uint8_t val = 0;

    public:
        constexpr Piece() : val(0){};
        constexpr Piece(uint8_t val) : val(val){};
        constexpr Piece(Side side, PieceTypes piece_val) : val(static_cast<uint8_t>(side) << 3 | static_cast<uint8_t>(piece_val)){};

        void set_value(uint8_t val) { this->val = val; };
        constexpr uint8_t get_value() const { return this->val; };
        constexpr PieceTypes get_type() const { return PieceTypes(get_bits(val, 2, 0)); };
        constexpr Side get_side() const { return Side(get_bit(val, 3)); };

        constexpr uint8_t to_bitboard_idx() const { return (2 * (static_cast<int>(get_type()) - 1)) + static_cast<uint8_t>(get_side()); };
};

inline bool operator==(const Piece& lhs, const Piece& rhs) { return lhs.get_value() == rhs.get_value(); }