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

    if (static_cast<int>(this->get_move_flags()) >= 8) {
        switch (static_cast<int>(this->get_move_flags()) & 0b0011) {
        case 0:
            to_return.push_back('r');
            break;
        case 1:
            to_return.push_back('n');
            break;
        case 2:
            to_return.push_back('b');
            break;
        case 3:
            to_return.push_back('q');
            break;
        default:
            break;
        }
    }

    return to_return;
}

bool operator==(const Move& lhs, const Move& rhs) { return lhs.get_move() == rhs.get_move(); }
