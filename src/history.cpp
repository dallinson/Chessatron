#include "history.hpp"

#include <algorithm>

HistoryValue HistoryTable::score(Move move, Side stm) const {
    if (!move.is_quiet()) {
        return 0;
    } else {
        return mainhist_score(move, stm);
    }
}

void HistoryTable::update_scores(const BoardHistory& hist, std::span<const ScoredMove> moves, ScoredMove current_move, Side stm, int depth) {
    if (!current_move.move.is_capture()) {
        update_mainhist_score(current_move.move, stm, bonus(depth));
    }
    std::for_each(moves.begin(), moves.end(), [&](ScoredMove move) {
        if (move.move.is_quiet()) {
            update_mainhist_score(move.move, stm, malus(depth));
        }
    });
}

void HistoryTable::update_mainhist_score(Move move, Side stm, HistoryValue bonus) {
    const auto scaled_bonus = bonus - mainhist_score(move, stm) * std::abs(bonus) / 32768;
    main_hist[move.hist_idx(stm)] += scaled_bonus;
}