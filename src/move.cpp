#include "move.hpp"

#include <string>

std::string Move::to_string() const {
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