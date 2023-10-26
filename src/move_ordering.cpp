#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"

size_t MoveOrdering::reorder_captures(MoveList& move_list, const ChessBoard& c) {
    size_t captures = 0;
    for (size_t i = 0; i < move_list.len(); i++) {
        if (move_list[i].is_capture()) {
            std::swap(move_list[captures], move_list[i]);
            captures += 1;
        }
    }
    std::sort(&move_list[0], &move_list[captures], [c](const Move a, const Move b) {
        int a_target = Evaluation::get_piece_score(c.get_piece(a.get_dest_square()).get_type());
        int a_source = Evaluation::get_piece_score(c.get_piece(a.get_src_square()).get_type());
        int b_target = Evaluation::get_piece_score(c.get_piece(b.get_dest_square()).get_type());
        int b_source = Evaluation::get_piece_score(c.get_piece(b.get_src_square()).get_type());
        return (a_target - a_source) > (b_target - b_source);
        // Reverse order to ensure that moves are in ascending order
    });
    return captures;
}