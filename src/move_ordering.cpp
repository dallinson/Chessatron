#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = {1, 2, 3, 4, 5, 6};

void MoveOrdering::reorder_moves(MoveList& moves, const ChessBoard& board, const Move pv_move, std::array<int32_t, 8192>& history_table, Move killer,
                                 bool& found_pv_move) {
    found_pv_move = false;
    for (size_t i = 0; i < moves.len(); i++) {
        moves[i].score = 0;
        if (moves[i].move == pv_move) {
            found_pv_move = true;
            moves[i].score = std::numeric_limits<int32_t>::max();
            continue;
        } else if (moves[i].move.is_promotion()) {
            switch (moves[i].move.get_promotion_piece_type()) {
            case PieceTypes::QUEEN:
                moves[i].score = 2000000001;
                break;
            case PieceTypes::KNIGHT:
                moves[i].score = 2000000000;
                break;
            default:
                moves[i].score = -2000000001;
                break;
            }
        } else if (moves[i].move.is_capture()) {
            moves[i].score = 900000000;
            moves[i].see_ordering_result = Search::static_exchange_evaluation(board, moves[i].move, -20);
            if (!moves[i].see_ordering_result) {
                moves[i].score = -1000000;
            }
            const auto src_score = ordering_scores[static_cast<uint8_t>(board.piece_at(moves[i].move.src_sq()).get_type()) - 1];
            const auto dest_type = moves[i].move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE
                                       ? PieceTypes::PAWN
                                       : board.piece_at(moves[i].move.dst_sq()).get_type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            moves[i].score += ((100000 * dest_score) + (6 - src_score));
        } else if (moves[i].move == killer) {
            moves[i].score = 800000000;
        } else {
            moves[i].score += history_table[moves[i].move.get_history_idx(board.get_side_to_move())];
        }
    }
    std::sort(&moves[0], &moves[moves.len()], [](const ScoredMove a, const ScoredMove b) { return a.score > b.score; });
}