#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = { 1, 5, 3, 3, 9, 20 };

void MoveOrdering::reorder_moves(MoveList& moves, const ChessBoard& board, const Move pv_move, bool& found_pv_move) {
    found_pv_move = false;
    for (size_t i = 0; i < moves.len(); i++) {
        moves[i].score = 0;
        if (moves[i].move == pv_move) {
            found_pv_move = true;
            moves[i].score = std::numeric_limits<int32_t>::max();
            continue;
        }
        if (moves[i].move.is_capture()) {
            moves[i].score += 1000000;
            const auto src_score = ordering_scores[static_cast<uint8_t>(board.get_piece(moves[i].move.get_src_square()).get_type()) - 1];
            const auto dest_type = moves[i].move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE ? PieceTypes::PAWN : board.get_piece(moves[i].move.get_dest_square()).get_type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            moves[i].score += 100 * dest_score;
            moves[i].score += (20 - src_score);
        }
        if (moves[i].move.get_move_flags() == MoveFlags::QUEEN_PROMOTION || moves[i].move.get_move_flags() == MoveFlags::QUEEN_PROMOTION_CAPTURE) {
            moves[i].score += 100000;
            moves[i].score += ordering_scores[static_cast<int>(moves[i].move.get_promotion_piece_type())];
        }
    }
    std::sort(&moves[0], &moves[moves.len()], [](const ScoredMove a, const ScoredMove b) {
        return a.score > b.score;
    });
}

/*size_t MoveOrdering::reorder_captures_first(MoveList& move_list, size_t start_pos) {
    size_t captures = start_pos;
    for (size_t i = start_pos; i < move_list.len(); i++) {
        if (move_list[i].move.is_capture()) {
            std::swap(move_list[captures], move_list[i]);
            captures += 1;
        }
    }
    return captures;
}

void MoveOrdering::sort_captures_mvv_lva(MoveList& move_list, const ChessBoard& c, const size_t capture_start, const size_t capture_count) {
    std::sort(&move_list[capture_start], &move_list[capture_start + capture_count], [c](const ScoredMove scored_a, const ScoredMove scored_b) {
        const auto a = scored_a.move;
        const auto b = scored_b.move;
        const auto a_target_type =
            a.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE ? PieceTypes::PAWN : c.get_piece(a.get_dest_square()).get_type();
        const auto b_target_type =
            b.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE ? PieceTypes::PAWN : c.get_piece(b.get_dest_square()).get_type();
        if (a_target_type != b_target_type) {
            return ordering_scores[static_cast<uint8_t>(a_target_type) - 1] > ordering_scores[static_cast<uint8_t>(b_target_type) - 1];
            // you're meant to use operator<; we use operator> in order to sort the victims in descending order
        }
        uint8_t a_src = ordering_scores[static_cast<uint8_t>(c.get_piece(a.get_src_square()).get_type()) - 1];
        uint8_t b_src = ordering_scores[static_cast<uint8_t>(c.get_piece(b.get_src_square()).get_type()) - 1];
        return a_src < b_src;
        // If the targets are equally valuable, sort the least valuable attacker first
    });
}

bool MoveOrdering::reorder_pv_move(MoveList& move_list, const Move pv_move) {
    for (size_t i = 0; i < move_list.len(); i++) {
        if (move_list[i].move == pv_move) {
            std::swap(move_list[0], move_list[i]);
            return true;
        }
    }
    return false;
}*/