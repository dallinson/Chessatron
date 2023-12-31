#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = { 1, 3, 3, 5, 9, 20 };

void MoveOrdering::reorder_moves(MoveList& moves, const ChessBoard& board, const Move pv_move, bool& found_pv_move, std::array<int32_t, 8192>& history_table) {
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
            moves[i].see_ordering_result = Search::static_exchange_evaluation(board, moves[i].move, -20);
            if (!moves[i].see_ordering_result) {
                moves[i].score -= 1000000;
            }
        } else {
            moves[i].score += history_table[moves[i].move.get_history_idx(board.get_side_to_move())];
        }
    }
    std::sort(&moves[0], &moves[moves.len()], [](const ScoredMove a, const ScoredMove b) {
        return a.score > b.score;
    });
}