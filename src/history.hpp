#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <vector>

#include "chessboard.hpp"
#include "move.hpp"
#include "utils.hpp"

using HistoryValue = int32_t;

class HistoryTable {
    private:
        std::array<HistoryValue, 8192> main_hist;
        std::array<std::vector<HistoryValue>, 4096> cont_hist;

        static size_t calc_hist_idx(Move move, Side stm) { return move.hist_idx(stm); };
        static HistoryValue bonus(int depth) { return std::min(16 * (depth + 1) * (depth + 1), 1200); };
        static HistoryValue malus(int depth) { return -bonus(depth); };

    public:
        HistoryTable() { 
            std::for_each(cont_hist.begin(), cont_hist.end(), [](auto& v) { v.resize(4096); });
            clear(); 
        };

        HistoryValue score(Move move, Side stm) const;
        HistoryValue mainhist_score(Move move, Side stm) const { return main_hist[move.hist_idx(stm)]; };
        void update_scores(const BoardHistory& hist, std::span<const ScoredMove> moves, ScoredMove current_move, Side stm, int depth);
        void update_mainhist_score(Move move, Side stm, HistoryValue bonus);
        void clear() { 
            std::for_each(cont_hist.begin(), cont_hist.end(), [](auto& v) { std::fill(v.begin(), v.end(), 0); });
            main_hist.fill(0); 
        };
};