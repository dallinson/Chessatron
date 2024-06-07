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
        std::vector<std::array<HistoryValue, 4096>> cont_hist;

        static size_t calc_hist_idx(Move move, Side stm) { return move.hist_idx(stm); };
        static HistoryValue bonus(int depth) { return std::min(16 * (depth + 1) * (depth + 1), 1200); };
        static HistoryValue malus(int depth) { return -bonus(depth); };

    public:
        HistoryTable() { 
            cont_hist.resize(4096);
            clear(); 
        };

        HistoryValue score(const BoardHistory& hist, Move move, Side stm) const;
        HistoryValue mainhist_score(Move move, Side stm) const { return main_hist[move.hist_idx(stm)]; };
        HistoryValue conthist_score(const BoardHistory& hist, Move move) const;
        void update_scores(const BoardHistory& hist, std::span<const Move> moves, ScoredMove current_move, Side stm, int depth);
        void update_mainhist_score(Move move, Side stm, HistoryValue bonus);
        void update_conthist_score(const BoardHistory& hist, Move move, HistoryValue bonus);
        void clear() { 
            std::for_each(cont_hist.begin(), cont_hist.end(), [](auto& arr) { arr.fill(0); });
            main_hist.fill(0); 
        };
};