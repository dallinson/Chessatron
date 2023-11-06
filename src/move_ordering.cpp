#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"

size_t MoveOrdering::reorder_captures_first(MoveList& move_list, size_t start_pos) {
    size_t captures = start_pos;
    for (size_t i = start_pos; i < move_list.len(); i++) {
        if (move_list[i].is_capture()) {
            std::swap(move_list[captures], move_list[i]);
            captures += 1;
        }
    }
    return captures;
}

void MoveOrdering::sort_captures_mvv_lva(MoveList& move_list, const ChessBoard& c, const size_t capture_start, const size_t capture_count) {
    std::sort(&move_list[capture_start], &move_list[capture_start + capture_count], [c](const Move a, const Move b) {
        const auto a_target_type =
            a.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE ? PieceTypes::PAWN : c.get_piece(a.get_dest_square()).get_type();
        const auto b_target_type =
            b.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE ? PieceTypes::PAWN : c.get_piece(b.get_dest_square()).get_type();
        if (a_target_type != b_target_type) {
            return Evaluation::get_piece_score(a_target_type) > Evaluation::get_piece_score(b_target_type);
            // you're meant to use operator<; we use operator> in order to sort the victims in descending order
        }
        int a_src = Evaluation::get_piece_score(c.get_piece(a.get_src_square()).get_type());
        int b_src = Evaluation::get_piece_score(c.get_piece(b.get_src_square()).get_type());
        return a_src < b_src;
        // If the targets are equally valuable, sort the least valuable attacker first
    });
}

bool MoveOrdering::reorder_pv_move(MoveList& move_list, const Move pv_move) {
    for (size_t i = 0; i < move_list.len(); i++) {
        if (move_list[i] == pv_move) {
            std::swap(move_list[0], move_list[i]);
            return true;
        }
    }
    return false;
}