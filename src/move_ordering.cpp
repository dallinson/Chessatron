#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = {1, 2, 3, 4, 5, 6};

void MovePicker::score_moves() {
    for (auto& move : moves) {
        move.score = 0;
        if (move.move == tt_move) {
            move.score = std::numeric_limits<int32_t>::max();
            //continue;
        } else if (move.move.is_promotion()) {
            switch (move.move.get_promotion_piece_type()) {
            case PieceTypes::QUEEN:
                move.score = 2000000001;
                break;
            case PieceTypes::KNIGHT:
                move.score = 2000000000;
                break;
            default:
                move.score = -2000000001;
                break;
            }
        } else if (move.move.is_capture()) {
            move.score = 900000000;
            move.see_ordering_result = Search::static_exchange_evaluation(board, move.move, -20);
            if (!move.see_ordering_result) {
                move.score = -1000000;
            }
            const auto src_score = ordering_scores[static_cast<uint8_t>(board.piece_at(move.move.src_sq()).get_type()) - 1];
            const auto dest_type = move.move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE
                                       ? PieceTypes::PAWN
                                       : board.piece_at(move.move.dst_sq()).get_type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            move.score += ((100000 * dest_score) + (6 - src_score));
        } else if (move.move == killer_move) {
            move.score = 800000000;
        } else {
            move.score = hist_table.score(board_hist, move.move, board.stm());
        }
    }
}


std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (stage == MovePickerStage::PICK_TT) {
        ScoredMove to_return;
        to_return.move = tt_move;
        stage = MovePickerStage::GEN_ALL;
        return to_return;
    } else if (stage == MovePickerStage::GEN_ALL) {
        idx = 0;
        stage = MovePickerStage::PICK_REMAINING;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(board, board.stm());
        score_moves();
    } else if (stage == MovePickerStage::GEN_QSEARCH) {
        idx = 0;
        stage = MovePickerStage::PICK_REMAINING;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIESCENCE>(board, board.stm());
        score_moves();
    }

    assert(stage == MovePickerStage::PICK_REMAINING);

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
    if (best_move.move == tt_move) {
        return next(skip_quiets);
    }
    return best_move;
}