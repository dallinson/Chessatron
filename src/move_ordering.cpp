#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = {1, 2, 3, 4, 5, 6};

MovePicker::MovePicker(const Position& pos, const BoardHistory& board_hist, const HistoryTable& hist_table, const Move tt_move, const Move killer_move, const bool is_qsearch) {
    this->idx = 0;
    
    for (auto& move : moves) {
        move.score = 0;
        if (move.move == tt_move) {
            move.score = std::numeric_limits<int32_t>::max();
            //continue;
        } else if (move.move.is_noisy()) {
            move.score = 900000000;
            move.see_ordering_result = Search::static_exchange_evaluation(pos, move.move, -20);
            if (!move.see_ordering_result) {
                move.score = -1000000;
            }
            const auto dest_type = (move.move.is_promotion() || move.move.flags() == MoveFlags::EN_PASSANT_CAPTURE)
                                       ? PieceTypes::PAWN
                                       : pos.piece_at(move.move.dst_sq()).get_type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            move.score += ((100000 * dest_score) + hist_table.capthist_score(board_hist, move.move));
        } else if (move.move == killer_move) {
            move.score = 800000000;
        } else {
            move.score += hist_table.score(board_hist, move.move, pos.stm());
        }
    }
}


std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (this->idx >= this->moves.size()) {
        return std::nullopt;
    }

    if (skip_quiets) {
        while (idx < this->moves.size() && moves[idx].move.is_quiet()) {
            idx += 1;
        }
    }

    if (idx >= this->moves.size()) {
        return std::nullopt;
    }

    int best_idx = this->idx;

    for (size_t i = (this->idx + 1); i < this->moves.size(); i++) {
        if (moves[i].score > moves[best_idx].score) {
            best_idx = i;
        }
    }
    std::swap(moves[best_idx], moves[idx]);

    const auto best_move = moves[idx];
    idx += 1;
    return best_move;
}