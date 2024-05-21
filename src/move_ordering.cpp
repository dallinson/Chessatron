#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

using enum MovePickerStage;

constexpr std::array<uint8_t, 6> ordering_scores = {1, 2, 3, 4, 5, 6};

void MovePicker::score_moves() {
    for (auto& move : moves) {
        move.score = 0;
        if (move.move.is_promotion()) {
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
            const auto src_score = ordering_scores[static_cast<uint8_t>(board.piece_at(move.move.src_sq()).get_type()) - 1];
            const auto dest_type = move.move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE
                                       ? PieceTypes::PAWN
                                       : board.piece_at(move.move.dst_sq()).get_type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            move.score += ((100000 * dest_score) + (6 - src_score));
        } else {
            move.score = hist_table.score(board_hist, move.move, board.stm());
        }
    }
}


std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (stage == MovePickerStage::PICK_TT) {
        ScoredMove to_return;
        to_return.move = tt_move;
        stage = MovePickerStage::GEN_GOOD_PROMOS;
        return to_return;
    } else if (stage == MovePickerStage::GEN_GOOD_PROMOS) {
        idx = 0;
        stage = MovePickerStage::PICK_GOOD_PROMOS;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::GOOD_PROMOS>(board, board.stm());
        score_moves();
    } else if (stage == MovePickerStage::GEN_CAPTURES) {
        idx = 0;
        stage = MovePickerStage::PICK_GOOD_CAPTURES;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::CAPTURES>(board, board.stm());
        score_moves();
    } else if (stage == MovePickerStage::PICK_KILLER) {
        stage = MovePickerStage::GEN_QUIETS;
        if (MoveGenerator::is_move_legal(board, killer_move) && MoveGenerator::is_move_pseudolegal(board, killer_move)) {
            ScoredMove to_return;
            to_return.move = killer_move;
            return to_return;
        } else {
            return next(skip_quiets);
        }
    } else if (stage == MovePickerStage::GEN_QUIETS) {
        idx = 0;
        stage = MovePickerStage::PICK_QUIETS;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIETS>(board, board.stm());
        score_moves();
    } else if (stage == PICK_BAD_CAPTURES_INIT) {
        idx = 0;
        stage = PICK_BAD_CAPTURES;
        return next(skip_quiets);
    } else if (stage == MovePickerStage::GEN_BAD_PROMOS) {
        idx = 0;
        stage = MovePickerStage::PICK_BAD_PROMOS;
        moves = MoveGenerator::generate_legal_moves<MoveGenType::BAD_PROMOS>(board, board.stm());
        score_moves();
    }

    if (skip_quiets && (stage == GEN_QUIETS || stage == PICK_QUIETS)) {
        stage = PICK_BAD_PROMOS;
        return next(skip_quiets);
    }

    MoveList& to_pick = (stage == PICK_BAD_CAPTURES) ? bad_capture_list : moves;

    if (idx >= to_pick.size() && stage != PICK_TT) {
        if (stage == PICK_GOOD_PROMOS) {
            stage = GEN_CAPTURES;
            return next(skip_quiets);
        } else if (stage == PICK_GOOD_CAPTURES) {
            stage = is_qsearch ? PICK_BAD_CAPTURES_INIT : PICK_KILLER;
            return next(skip_quiets);
        } else if (stage == PICK_QUIETS) {
            stage = PICK_BAD_CAPTURES_INIT;
            return next(skip_quiets);
        } else if (stage == PICK_BAD_CAPTURES) {
            stage = is_qsearch ? NO_MOVES : GEN_BAD_PROMOS;
            return next(skip_quiets); 
        } else if (stage == PICK_BAD_PROMOS) {
            stage = NO_MOVES;
            return std::nullopt;
        } else {
            return std::nullopt;
        }
    }

    int best_idx = this->idx;

    for (size_t i = (this->idx + 1); i < to_pick.size(); i++) {
        if (to_pick[i].score > to_pick[best_idx].score) {
            best_idx = i;
        }
    }
    std::swap(to_pick[best_idx], to_pick[idx]);

    auto best_move = to_pick[idx];
    idx += 1;
    if (best_move.move == tt_move || best_move.move == killer_move) {
        return next(skip_quiets);
    }
    if (stage == PICK_GOOD_CAPTURES) {
        best_move.see_ordering_result = Search::static_exchange_evaluation(board, best_move.move, -20);
        if (!best_move.see_ordering_result) {
            bad_capture_list.add_move(best_move);
            return next(skip_quiets);
        }
    }
    return best_move;
}