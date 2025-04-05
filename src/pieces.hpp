#pragma once

#include "utils.hpp"

#define WHITE_IDX 0
#define BLACK_IDX 1

enum class PieceTypes : uint8_t {
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6
};

class Piece {
    private:
        uint8_t _val = 0;

    public:
        constexpr Piece() : _val(0){};
        constexpr Piece(uint8_t val) : _val(val){};
        constexpr Piece(Side side, PieceTypes piece_val) : _val(static_cast<uint8_t>(side) << 3 | static_cast<uint8_t>(piece_val)){};

        void set_value(uint8_t val) { this->_val = val; };
        constexpr uint8_t val() const { return this->_val; };
        constexpr PieceTypes type() const { return PieceTypes(get_bits(_val, 2, 0)); };
        constexpr Side side() const { return Side(get_bit(_val, 3)); };

        constexpr uint8_t to_bitboard_idx() const { return (2 * (static_cast<int>(type()) - 1)) + static_cast<uint8_t>(side()); };
};

inline bool operator==(const Piece& lhs, const Piece& rhs) { return lhs.val() == rhs.val(); }