#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 7> ordering_scores = {0, 1, 2, 3, 4, 5, 6};

MovePicker::MovePicker(MoveList&& input_moves, const Position& pos, const BoardHistory& hist, const Move pv_move, const HistoryTable& history_table, Move killer) {
    this->moves = input_moves;
    this->start_idx = 0;

    auto best_idx = 0;
    
    for (size_t i = 0; i < moves.size(); i++) {
        auto& move = moves[i];
        move.score = 0;
        if (move.move == pv_move) {
            move.score = std::numeric_limits<int32_t>::max();
            move.stage = MovePickerStage::TT_MOVE;
            //continue;
        } else if (move.move.is_noisy()) {
            move.stage = MovePickerStage::GOOD_NOISY;
            move.see_ordering_result = Search::static_exchange_evaluation(pos, move.move, -20);
            if (!move.see_ordering_result) {
                move.stage = MovePickerStage::BAD_NOISY;
            }
            const auto dest_type = move.move.flags() == MoveFlags::EN_PASSANT_CAPTURE
                                       ? PieceTypes::PAWN
                                       : pos.piece_at(move.move.dst_sq()).type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type)];
            move.score += ((100000 * dest_score) + history_table.capthist_score(hist, move.move));
        } else if (move.move == killer) {
            move.stage = MovePickerStage::KILLER;
        } else {
            move.score += history_table.score(hist, move.move, pos.stm());
            move.stage = MovePickerStage::QUIET;
        }
        stage = std::min(stage, move.stage);
    }
    std::swap(moves[0], moves[best_idx]);
}


std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (start_idx >= moves.size()) { // if we've evaluated all moves
        stage = MovePickerStage::NONE;
    }
    if (skip_quiets && stage == MovePickerStage::QUIET) {
        stage = next_stage(MovePickerStage::QUIET);
    }
    if (stage == MovePickerStage::NONE) { // fallthrough
        return std::nullopt;
    }

    int lowest_move_idx = -1;
    for (size_t i = start_idx; i < moves.size(); i++) {
        if (moves[i].stage == stage) {
            lowest_move_idx = i;
            break;
        }
    }
    if (lowest_move_idx == -1) {
        stage = next_stage(stage);
        return next(skip_quiets);
    }

    i32 best_idx = lowest_move_idx;
    for (size_t i = (lowest_move_idx + 1); i < moves.size(); i++) {
        if (moves[i].stage == stage && moves[i].score > moves[best_idx].score) {
            best_idx = i;
        }
    }
    std::swap(moves[best_idx], moves[start_idx]);

    const auto best_move = moves[start_idx];
    start_idx += 1;
    return best_move;
}