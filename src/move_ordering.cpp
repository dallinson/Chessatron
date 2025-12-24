#include "move_ordering.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"


auto MovePicker::score_noisies() -> void {
    for (auto& move : moves) {
        const auto dest_type = move.move.flags() == MoveFlags::EN_PASSANT_CAPTURE
                                    ? PieceTypes::PAWN
                                    : pos.piece_at(move.move.dst_sq()).type();
        const auto dest_score = Search::SEEScores[static_cast<u8>(dest_type)];
        move.score = hist_table.capthist_score(board_hist, move.move) / 16;
        move.score += dest_score;
        if (move.move.is_promotion()) {
            move.score += (Search::SEEScores[static_cast<u8>(move.move.promo_type())] - Search::SEEScores[static_cast<u8>(PieceTypes::PAWN)]);
        }
    }
}

auto MovePicker::score_quiets() -> void {
    for (auto& move : moves) {
        move.score = hist_table.score(board_hist, move.move, pos.stm());
    }
}

auto MovePicker::pick_good_noisies() -> std::optional<ScoredMove> {
    auto opt_move = pick_move(moves);
    if (!opt_move.has_value()) {
        return std::nullopt; // because we've evaluated all noisies
    } 
    auto move = opt_move.value();
    move.see_ordering_result = Search::static_exchange_evaluation(pos, move.move, -move.score / 4);
    if (move.see_ordering_result) {
        return move;
    } else {
        bad_noisies.add(move);
        return pick_good_noisies(); // keep trying until we get a good one
    }
}

// Gets the next move with the highest possible score in this list
auto MovePicker::pick_move(MoveList& moves_to_search) -> std::optional<ScoredMove> {
    if (idx >= moves_to_search.size()) {
        return std::nullopt;
    }

    auto best_idx = idx;
    for (usize i = idx + 1; i < moves_to_search.size(); i++) {
        // No need to check idx; we can swap with itself
        if (moves_to_search[i].score > moves_to_search[best_idx].score) {
            best_idx = i;
        }
    }
    std::swap(moves_to_search[idx], moves_to_search[best_idx]);
    const auto best_move = moves_to_search[idx];
    idx += 1;
    return best_move;
}

std::optional<ScoredMove> MovePicker::next(const bool skip_quiets) {
    if (stage == MovePickerStage::TT_MOVE) {
        stage = MovePickerStage::GEN_NOISY;
        if (tt_move.is_null_move() || !(MoveGenerator::is_move_pseudolegal(pos, tt_move) && MoveGenerator::is_move_legal(pos, tt_move))) {
            // Make sure the move is legal in this position
            return next(skip_quiets);
        }
        assert(!tt_move.is_null_move());
        return ScoredMove(tt_move);
    } else if (stage == MovePickerStage::GEN_NOISY) {
        moves = MoveGenerator::generate_legal_moves<MoveGenType::NOISY>(pos, pos.stm());
        stage = MovePickerStage::PICK_GOOD_NOISY;
        idx = 0;
        score_noisies();
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_GOOD_NOISY) {
        const auto to_return = pick_good_noisies();
        if (!to_return.has_value()) {
            stage = is_quiescence ? MovePickerStage::GEN_BAD_NOISY : MovePickerStage::KILLER;
            return next(skip_quiets);
        } else if (to_return.value().move == tt_move) {
            return next(skip_quiets);
        }
        assert(!to_return->move.is_null_move());
        return to_return;
    } else if (stage == MovePickerStage::KILLER) {
        if (is_quiescence) {
            // If the qsearch is in check it's considered to _not_ be a quiescent search
            stage = MovePickerStage::GEN_BAD_NOISY;
        } else {
            stage = MovePickerStage::GEN_QUIET;
        }
        if (killer_move == tt_move || killer_move.is_null_move() || !(MoveGenerator::is_move_pseudolegal(pos, killer_move) && MoveGenerator::is_move_legal(pos, killer_move))) {
            return next(skip_quiets);
        }
        return ScoredMove(killer_move);
    } else if (stage == MovePickerStage::GEN_QUIET) {
        if (skip_quiets) {
            stage = MovePickerStage::GEN_BAD_NOISY;
            return next(skip_quiets);
        }
        moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIETS>(pos, pos.stm());
        stage = MovePickerStage::PICK_QUIET;
        idx = 0;
        score_quiets();
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_QUIET) {
        if (skip_quiets) {
            stage = MovePickerStage::GEN_BAD_NOISY;
            return next(skip_quiets);
        }
        const auto to_return = pick_move(moves);
        if (!to_return.has_value()) {
            stage = MovePickerStage::GEN_BAD_NOISY;
            return next(skip_quiets);
        }
        if (to_return->move == tt_move || to_return->move == killer_move) {
            return next(skip_quiets);
        }
        assert(!to_return->move.is_null_move());
        return to_return;
    } else if (stage == MovePickerStage::GEN_BAD_NOISY) {
        stage = MovePickerStage::PICK_BAD_NOISY;
        idx = 0;
        return next(skip_quiets);
    } else if (stage == MovePickerStage::PICK_BAD_NOISY) {
        const auto to_return = pick_move(bad_noisies);
        if (!to_return.has_value()) {
            stage = MovePickerStage::NONE;
            return std::nullopt;
        }
        if (to_return->move == tt_move || to_return->move == killer_move) {
            return next(skip_quiets);
        }
        assert(!to_return->move.is_null_move());
        return to_return;
    } else {
        return std::nullopt;
    }
}