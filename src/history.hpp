#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "move.hpp"
#include "utils.hpp"

using HistoryValue = int32_t;

class HistoryTable {
    private:
        std::array<HistoryValue, 8192> main_hist;

        static size_t calc_hist_idx(Move move, Side stm) { return move.hist_idx(stm); };
        static HistoryValue bonus(int depth) { return std::min(16 * (depth + 1) * (depth + 1), 1200); };
        static HistoryValue malus(int depth) { return -bonus(depth); };

    public:
        HistoryTable() { clear(); };

        HistoryValue score(Move move, Side stm) const;
        HistoryValue mainhist_score(Move move, Side stm) const { return main_hist[move.hist_idx(stm)]; };
        void update_scores(std::span<const ScoredMove> moves, ScoredMove current_move, Side stm, int depth);
        void update_mainhist_score(Move move, Side stm, HistoryValue bonus);
        void clear() { main_hist.fill(0); };
};