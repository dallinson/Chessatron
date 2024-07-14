#include "history.hpp"

#include <algorithm>

#include "search.hpp"

HistoryValue HistoryTable::score(const BoardHistory& hist, Move move, Side stm) const {
    if (move.is_noisy()) {
        return capthist_score(hist, move);
    } else {
        return mainhist_score(move, stm) + 2 * conthist_score(hist, move);
    }
}

void HistoryTable::update_scores(const BoardHistory& hist, std::span<const Move> moves, ScoredMove current_move, Side stm, int depth) {
    const auto hist_bonus = bonus(depth);
    const auto hist_malus = malus(depth);
    if (!current_move.move.is_noisy()) {
        update_mainhist_score(current_move.move, stm, hist_bonus);
        update_conthist_score(hist, current_move.move, hist_bonus);
        std::for_each(moves.begin(), moves.end(), [&](Move move) {
            if (!move.is_noisy()) {
                update_mainhist_score(move, stm, hist_malus);
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

void HistoryTable::update_mainhist_score(Move move, Side stm, HistoryValue bonus) {
    const auto scaled_bonus = bonus - mainhist_score(move, stm) * std::abs(bonus) / 32768;
    main_hist[move.hist_idx(stm)] += scaled_bonus;
}


void HistoryTable::update_conthist_score(const BoardHistory& hist, Move move, HistoryValue bonus) {
    if (!hist.move_at(hist.len() - 1).is_null_move()) {
        const auto scaled_bonus = bonus - conthist_score(hist, move) * std::abs(bonus) / 32768;
        (*cont_hist)[hist[hist.len() - 2].piece_to(hist.move_at(hist.len() - 1))][hist[hist.len() - 1].piece_to(move)] += scaled_bonus;
    }
}

HistoryValue HistoryTable::conthist_score(const BoardHistory& hist, Move move) const {
    if (!hist.move_at(hist.len() - 1).is_null_move()) {
        return (*cont_hist)[hist[hist.len() - 2].piece_to(hist.move_at(hist.len() - 1))][hist[hist.len() - 1].piece_to(move)];
    } else {
        return 0;
    }
}


HistoryValue HistoryTable::capthist_score(const BoardHistory& hist, const Move move) const {
    const auto& pos = hist[hist.len() - 1];
    const auto captured_type = (move.is_promotion() || move.flags() == MoveFlags::EN_PASSANT_CAPTURE)
        ? PieceTypes::PAWN
        : pos.piece_at(move.dst_sq()).type();
    return (*capt_hist)[pos.piece_to(move)][static_cast<int>(captured_type) - 1];
}

void HistoryTable::update_capthist_score(const BoardHistory& hist, Move move, HistoryValue bonus) {
    const auto& pos = hist[hist.len() - 1];
    const auto captured_type = (move.is_promotion() || move.flags() == MoveFlags::EN_PASSANT_CAPTURE)
        ? PieceTypes::PAWN
        : pos.piece_at(move.dst_sq()).type();
    const auto scaled_bonus = bonus - capthist_score(hist, move) * std::abs(bonus) / 32768;
    (*capt_hist)[pos.piece_to(move)][static_cast<int>(captured_type) - 1] += scaled_bonus;
}

int corrhist_idx(const ZobristKey pawn_hash) {
    return pawn_hash & (16384 - 1);
}


Score HistoryTable::corrhist_score(const Position& pos, const Score static_eval) const {
    const Score entry = (*corr_hist)[corrhist_idx(pos.pawn_hash())][static_cast<int>(pos.stm())];
    const int32_t adjusted_score = static_eval + (entry * std::abs(entry)) / 16384;

    return std::clamp(adjusted_score, -MATE_FOUND + 1, MATE_FOUND - 1);
}

void HistoryTable::update_corrhist_score(const Position& pos, const Score static_eval, const Score search_score, const int depth) {
    
    const auto error = search_score - static_eval;
    const auto bonus = std::clamp(error * depth / 8, -128, 128);
    auto& score = (*corr_hist)[corrhist_idx(pos.pawn_hash())][static_cast<int>(pos.stm())];
    score += bonus - score * std::abs(bonus) / 512;
}