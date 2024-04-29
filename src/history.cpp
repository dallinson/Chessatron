#include "history.hpp"

#include <algorithm>

HistoryValue HistoryTable::get_score(Move move, Side stm) const {
    if (!move.is_quiet()) {
        return 0;
    } else {
        return main_hist[move.hist_idx(stm)];
    }
}

void HistoryTable::update_scores(std::span<const ScoredMove> moves, ScoredMove current_move, Side stm, int depth) {
    if (!current_move.move.is_capture()) {
        main_hist[current_move.move.hist_idx(stm)] += bonus(depth);
    }
    std::for_each(moves.begin(), moves.end(), [&](ScoredMove move) {
        if (move.move.is_quiet()) {
            main_hist[move.move.hist_idx(stm)] += malus(depth);
        }
    });
}