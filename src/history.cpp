#include "history.hpp"

#include <algorithm>

HistoryValue HistoryTable::score(const BoardHistory& hist, Move move, Side stm) const {
    if (!move.is_quiet()) {
        return 0;
    } else {
        return mainhist_score(move, stm) + 2 * conthist_score(hist, move);
    }
}

void HistoryTable::update_scores(const BoardHistory& hist, std::span<const ScoredMove> moves, ScoredMove current_move, Side stm, int depth) {
    const auto hist_bonus = bonus(depth);
    const auto hist_malus = malus(depth);
    if (!current_move.move.is_capture()) {
        update_mainhist_score(current_move.move, stm, hist_bonus);
        update_conthist_score(hist, current_move.move, hist_bonus);
    }
    std::for_each(moves.begin(), moves.end(), [&](ScoredMove move) {
        if (move.move.is_quiet()) {
            update_mainhist_score(move.move, stm, hist_malus);
            update_conthist_score(hist, move.move, hist_malus);
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
        cont_hist[hist[hist.len() - 2].piece_to(hist.move_at(hist.len() - 1))][hist[hist.len() - 1].piece_to(move)] += scaled_bonus;
    }
}

HistoryValue HistoryTable::conthist_score(const BoardHistory& hist, Move move) const {
    if (!hist.move_at(hist.len() - 1).is_null_move()) {
        return cont_hist[hist[hist.len() - 2].piece_to(hist.move_at(hist.len() - 1))][hist[hist.len() - 1].piece_to(move)];
    } else {
        return 0;
    }
}