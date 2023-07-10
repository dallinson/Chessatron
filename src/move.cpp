#include "move.hpp"

#include <string>

std::string Move::to_string() const {
    std::string to_return;
    to_return.push_back(this->get_src_file() + 97);
    to_return.push_back(this->get_src_rank() + 49);


    to_return.push_back(this->get_dest_file() + 97);
    to_return.push_back(this->get_dest_rank() + 49);

    return to_return;
}