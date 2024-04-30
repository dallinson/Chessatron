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
        static HistoryValue bonus(int depth) { return depth * depth; };
        static HistoryValue malus(int depth) { return -bonus(depth); };

    public:
        HistoryTable() { reset(); };

        HistoryValue get_score(Move move, Side stm) const;
        void update_scores(std::span<const ScoredMove> moves, ScoredMove current_move, Side stm, int depth);
        void reset() { main_hist.fill(0); };
};