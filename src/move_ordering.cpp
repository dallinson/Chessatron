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

auto MovePicker::pick_move(const bool skip_quiets) -> std::optional<ScoredMove> {
    if (this->idx >= this->moves.size()) {
        return std::nullopt; // If we're past the end of the moves
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

std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (stage == MovePickerStage::PICK_TT) {
        stage = MovePickerStage::GEN_MOVES;
        return ScoredMove(_tt_move);
    } else if (stage == MovePickerStage::GEN_MOVES) {
        if (_is_qsearch && !_pos.in_check()) {
            moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIESCENCE>(_pos, _pos.stm());
        } else {
            moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(_pos, _pos.stm());
        }
        idx = 0;
        score_moves();
        stage = MovePickerStage::PICK_MOVES;
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_MOVES) {
        const auto move = pick_move(skip_quiets);
        if (move.has_value() && move->move == _tt_move) {
            return next(skip_quiets);
        } else {
            return move; // Either std::nullopt or the move isn't the tt move
        }
    } else {
        __builtin_unreachable();
    }
}