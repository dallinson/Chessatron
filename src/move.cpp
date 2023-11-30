#include "move.hpp"

#include <string>

const Move Move::NULL_MOVE(0);

std::string Move::to_string() const {
    if (this->get_src_square() == 0 && this->get_dest_square() == 0) {
        // if this is a null move
        return "0000";
    }
    std::string to_return;
    to_return.push_back(this->get_src_file() + 97);
    to_return.push_back(this->get_src_rank() + 49);

    to_return.push_back(this->get_dest_file() + 97);
    to_return.push_back(this->get_dest_rank() + 49);

    if (this->is_promotion()) {
        switch (this->get_promotion_piece_type()) {
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

bool operator==(const Move& lhs, const Move& rhs) { return lhs.get_move() == rhs.get_move(); }
