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
        stage = MovePickerStage::GEN_NOISY;
        return tt_move;
    } else if (stage == MovePickerStage::GEN_NOISY) {
        stage = MovePickerStage::PICK_GOOD_NOISY;
        idx = 0;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::NOISY>(pos, pos.stm());
        score_moves();
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_GOOD_NOISY) {
        if (idx >= moves.size()) {
            stage = is_qsearch ? MovePickerStage::START_BAD_NOISY : MovePickerStage::GEN_QUIETS;
            return next(skip_quiets);
        }

        auto best_move = partial_selection_sort();
        idx += 1;
        if (best_move.move == tt_move) return next(skip_quiets);
        best_move.see_ordering_result = Search::static_exchange_evaluation(pos, best_move.move, -20);
        if (best_move.see_ordering_result) {
            return best_move;
        }
        bad_captures.add(best_move);
        return next(skip_quiets);
    } else if (stage == MovePickerStage::GEN_QUIETS) {
        if (skip_quiets) {
            stage = MovePickerStage::START_BAD_NOISY;
            return next(skip_quiets);
        }
        stage = MovePickerStage::PICK_QUIETS;
        idx = 0;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIETS>(pos, pos.stm());
        score_moves();
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_QUIETS) {
        if (skip_quiets || idx >= moves.size()) {
            stage = MovePickerStage::START_BAD_NOISY;
            return next(skip_quiets);
        }

        const auto best_move = partial_selection_sort();
        idx += 1;
        if (best_move.move == tt_move) return next(skip_quiets);
        return best_move;
    } else if (stage == MovePickerStage::START_BAD_NOISY) {
        idx = 0;
        moves = bad_captures;
        stage = MovePickerStage::PICK_BAD_NOISY;
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_BAD_NOISY) {
        if (idx >= moves.size()) {
            stage = MovePickerStage::PICK_NONE;
            return std::nullopt;
        }
        const auto best_move = partial_selection_sort();
        idx += 1;
        if (best_move.move == tt_move) return next(skip_quiets);
        return best_move;
    } else if (stage == MovePickerStage::PICK_NONE) {
        return std::nullopt;
    }
    return std::nullopt;
}