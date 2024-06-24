#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = {1, 2, 3, 4, 5, 6};

void MovePicker::score_moves() {
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

ScoredMove MovePicker::partial_selection_sort() {
    int best_idx = this->idx;

    for (size_t i = (this->idx + 1); i < this->moves.size(); i++) {
        if (moves[i].score > moves[best_idx].score) {
            best_idx = i;
        }
    }
    std::swap(moves[best_idx], moves[idx]);
    return moves[idx];
}

std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (stage == MovePickerStage::PICK_TT) {
        stage = MovePickerStage::GEN_ALL;
        return tt_move;
    } else if (stage == MovePickerStage::GEN_ALL) {
        stage = MovePickerStage::PICK_REMAINING;
        idx = 0;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, pos.stm());
        score_moves();
        return next(skip_quiets);
    } else if (stage == MovePickerStage::GEN_QSEARCH) {
        stage = MovePickerStage::PICK_REMAINING;
        idx = 0;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIESCENCE>(pos, pos.stm());
        score_moves();
        return next(skip_quiets);
    } else {
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

        const auto best_move = partial_selection_sort();
        idx += 1;
        if (best_move.move == tt_move) return next(skip_quiets);
        return best_move;
    }
}