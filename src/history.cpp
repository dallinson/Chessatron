#include "history.hpp"

#include <algorithm>

#include "search.hpp"

HistoryValue HistoryTable::score(const BoardHistory& hist, Move move, Side stm) const {
    if (move.is_noisy()) {
        return capthist_score(hist, move);
    } else {
        return mainhist_score(hist.boards_back(0), move, stm) + 2 * conthist_score(hist, move);
    }
}

void HistoryTable::update_scores(const BoardHistory& hist, std::span<const Move> moves, ScoredMove current_move, Side stm, int depth) {
    const auto hist_bonus = bonus(depth);
    const auto hist_malus = malus(depth);
    if (!current_move.move.is_noisy()) {
        update_mainhist_score(hist.boards_back(0), current_move.move, stm, hist_bonus);
        update_conthist_score(hist, current_move.move, hist_bonus);
        std::for_each(moves.begin(), moves.end(), [&](Move move) {
            if (!move.is_noisy()) {
                update_mainhist_score(hist.boards_back(0), move, stm, hist_malus);
                update_conthist_score(hist, move, hist_malus);
            }
        });
    } else {
        update_capthist_score(hist, current_move.move, hist_bonus);
    }
    std::for_each(moves.begin(), moves.end(), [&](Move move) {
        if (move.is_noisy() && move != current_move.move) {
            update_capthist_score(hist, move, hist_malus);
        }
    });
}

void HistoryTable::update_mainhist_score(const Position& pos, Move move, Side stm, HistoryValue bonus) {
    const auto fromto_scaled_bonus = bonus - fromto_mainhist[move.fromto(stm)] * std::abs(bonus) / 32768;
    fromto_mainhist[move.fromto(stm)] += fromto_scaled_bonus;
    const auto pieceto_scaled_bonus = bonus - pieceto_mainhist[pos.pieceto(move)] * std::abs(bonus) / 32768;
    pieceto_mainhist[pos.pieceto(move)] += pieceto_scaled_bonus;
}


void HistoryTable::update_conthist_score(const BoardHistory& hist, Move move, HistoryValue bonus) {
    if (!hist.moves_back(0).is_null_move()) {
        const auto scaled_bonus = bonus - conthist_score(hist, move) * std::abs(bonus) / 32768;
        (*cont_hist)[hist.boards_back(1).pieceto(hist.moves_back(0))][hist.boards_back(0).pieceto(move)] += scaled_bonus;
    }
}

HistoryValue HistoryTable::conthist_score(const BoardHistory& hist, Move move) const {
    if (!hist.moves_back(0).is_null_move()) {
        return (*cont_hist)[hist.boards_back(1).pieceto(hist.moves_back(0))][hist.boards_back(0).pieceto(move)];
    } else {
        return 0;
    }
}

HistoryValue HistoryTable::capthist_score(const BoardHistory& hist, const Move move) const {
    const auto& pos = hist.boards_back(0);
    const auto captured_type = [&]() {
        if (move.is_promotion()) {
            return move.promo_type();
        } else if (move.flags() == MoveFlags::EN_PASSANT_CAPTURE) {
            return PieceTypes::PAWN;
        } else {
            return pos.piece_at(move.dst_sq()).type();
        }
    }();
    return (*capt_hist)[pos.pieceto(move)][static_cast<int>(captured_type) - 1];
}

void HistoryTable::update_capthist_score(const BoardHistory& hist, Move move, HistoryValue bonus) {
    const auto& pos = hist.boards_back(0);
    const auto captured_type = [&]() {
        if (move.is_promotion()) {
            return move.promo_type();
        } else if (move.flags() == MoveFlags::EN_PASSANT_CAPTURE) {
            return PieceTypes::PAWN;
        } else {
            return pos.piece_at(move.dst_sq()).type();
        }
    }();
    const auto scaled_bonus = bonus - capthist_score(hist, move) * std::abs(bonus) / 32768;
    (*capt_hist)[pos.pieceto(move)][static_cast<int>(captured_type) - 1] += scaled_bonus;
}

int corrhist_idx(const ZobristKey pawn_hash) {
    return pawn_hash & (16384 - 1);
}


Score HistoryTable::corrhist_score(const Position& pos, const Score static_eval, const BoardHistory& hist) const {
    Score entry = (*pawn_corr_hist)[corrhist_idx(pos.pawn_hash())][static_cast<int>(pos.stm())];
    entry += (*white_non_pawn_corr_hist)[corrhist_idx(pos.white_non_pawn_hash())][static_cast<int>(pos.stm())];
    entry += (*black_non_pawn_corr_hist)[corrhist_idx(pos.black_non_pawn_hash())][static_cast<int>(pos.stm())];
    if (hist.len() >= 3 && !hist.moves_back(1).is_null_move() && !hist.moves_back(0).is_null_move()) {
        entry += (*cont_corr_hist)[hist.boards_back(2).pieceto(hist.moves_back(1))][hist.boards_back(1).pieceto(hist.moves_back(0))];
    }

    const i32 adjusted_score = static_eval + (entry * std::abs(entry)) / 16384;

    return std::clamp(adjusted_score, MATED_IN_MAX_PLY + 1, MATE_IN_MAX_PLY - 1);
}

void HistoryTable::update_corrhist_score(const Position& pos, const Score static_eval, const Score search_score, const int depth, const BoardHistory& hist) {
    const auto error = search_score - static_eval;
    const auto bonus = std::clamp(error * depth / 8, -128, 128);
    auto& pawn_score = (*pawn_corr_hist)[corrhist_idx(pos.pawn_hash())][static_cast<int>(pos.stm())];
    pawn_score += bonus - pawn_score * std::abs(bonus) / 512;
    auto& white_non_pawn_score = (*white_non_pawn_corr_hist)[corrhist_idx(pos.white_non_pawn_hash())][static_cast<int>(pos.stm())];
    white_non_pawn_score += bonus - white_non_pawn_score * std::abs(bonus) / 512;
    auto& black_non_pawn_score = (*black_non_pawn_corr_hist)[corrhist_idx(pos.black_non_pawn_hash())][static_cast<int>(pos.stm())];
    black_non_pawn_score += bonus - black_non_pawn_score * std::abs(bonus) / 512;
    if (hist.len() >= 3 && !hist.moves_back(1).is_null_move() && !hist.moves_back(0).is_null_move()) {
        auto& cont_corr_hist_score = (*cont_corr_hist)[hist.boards_back(2).pieceto(hist.moves_back(1))][hist.boards_back(1).pieceto(hist.moves_back(0))];
        cont_corr_hist_score += bonus - cont_corr_hist_score * std::abs(bonus) / 512;
    }
}