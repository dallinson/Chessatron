#include "move_ordering.hpp"

#include <algorithm>
#include <array>

size_t MoveOrdering::reorder_captures(MoveList& move_list) {
    size_t captures = 0;
    for (size_t i = 0; i < move_list.len(); i++) {
        if (move_list[i].is_capture()) {
            std::swap(move_list[captures], move_list[i]);
            captures += 1;
        }
    }
    return captures;
}