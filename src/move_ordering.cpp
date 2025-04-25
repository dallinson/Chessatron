#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = {1, 2, 3, 4, 5, 6};

auto MovePicker::score_moves() -> void {
    auto best_idx = 0;
    
    for (size_t i = 0; i < moves.size(); i++) {
        auto& move = moves[i];
        move.score = 0;
        if (move.move == _tt_move) {
            move.score = std::numeric_limits<int32_t>::max();
            //continue;
        } else if (move.move.is_noisy()) {
            move.score = 900000000;
            move.see_ordering_result = Search::static_exchange_evaluation(_pos, move.move, -20);
            if (!move.see_ordering_result) {
                move.score = -1000000;
            }
            const auto dest_type = (move.move.is_promotion() || move.move.flags() == MoveFlags::EN_PASSANT_CAPTURE)
                                       ? PieceTypes::PAWN
                                       : _pos.piece_at(move.move.dst_sq()).type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            move.score += ((100000 * dest_score) + _history_table.capthist_score(_board_hist, move.move));
        } else if (move.move == _killer_move) {
            move.score = 800000000;
        } else {
            move.score += _history_table.score(_board_hist, move.move, _pos.stm());
        }
        if (move.score > moves[best_idx].score) {
            best_idx = i;
        }
    }
    std::swap(moves[0], moves[best_idx]);
}


std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (this->idx >= this->moves.size()) {
        return std::nullopt;
    } else if (this->idx == 0) {
        this->idx += 1;
        return std::optional(this->moves[0]);
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