#pragma once

#include <cctype>

#include "utils.hpp"

#define WHITE_IDX 0
#define BLACK_IDX 1

enum class PieceTypes : uint8_t {
    NONE = 0,
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

        constexpr auto to_fen() const -> char {
            auto to_return = '.';
            if (type() == PieceTypes::PAWN) {
                to_return = 'p';
            } else if (type() == PieceTypes::KNIGHT) {
                to_return = 'n';
            } else if (type() == PieceTypes::BISHOP) {
                to_return = 'b';
            } else if (type() == PieceTypes::ROOK) {
                to_return = 'r';
            } else if (type() == PieceTypes::QUEEN) {
                to_return = 'q';
            } else if (type() == PieceTypes::KING) {
                to_return = 'k';
            }
            if (side() == Side::WHITE) {
                to_return = std::toupper(to_return);
            }
            return to_return;
        }
};

inline bool operator==(const Piece& lhs, const Piece& rhs) { return lhs.val() == rhs.val(); }