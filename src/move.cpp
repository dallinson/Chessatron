#include "move.hpp"

#include <string>


std::string Move::to_string() const {
    if (this->src_sq() == Square::A1 && this->dst_sq() == Square::A1) {
        // if this is a null move
        return "0000";
    }
    std::string to_return;
    to_return.push_back(this->src_fle() + 97);
    to_return.push_back(this->src_rnk() + 49);

    to_return.push_back(this->dst_fle() + 97);
    to_return.push_back(this->dst_rnk() + 49);

    if (this->is_promotion()) {
        switch (this->promo_type()) {
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

    return to_return;
}

bool operator==(const Move& lhs, const Move& rhs) { return lhs.value() == rhs.value(); }
