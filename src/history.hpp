#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

#include "chessboard.hpp"
#include "mdarray.hpp"
#include "move.hpp"
#include "utils.hpp"

using HistoryValue = int32_t;

class HistoryTable {
    private:
        std::array<HistoryValue, 8192> main_hist;
        std::unique_ptr<MDArray<HistoryValue, 1024, 1024>> cont_hist;
        std::unique_ptr<MDArray<HistoryValue, 1024, 6>> capt_hist;
        std::unique_ptr<MDArray<Score, 16384, 2>> pawn_corr_hist;
        std::unique_ptr<MDArray<Score, 16384, 2>> white_non_pawn_corr_hist;
        std::unique_ptr<MDArray<Score, 16384, 2>> black_non_pawn_corr_hist;
        std::unique_ptr<MDArray<Score, 1024, 1024>> cont_corr_hist;

        static size_t calc_hist_idx(Move move, Side stm) { return move.hist_idx(stm); };
        static HistoryValue bonus(int depth) { return std::min(16 * (depth + 1) * (depth + 1), 1200); };
        static HistoryValue malus(int depth) { return -bonus(depth); };

    public:
        HistoryTable() {
            cont_hist = std::make_unique<MDArray<HistoryValue, 1024, 1024>>();
            capt_hist = std::make_unique<MDArray<HistoryValue, 1024, 6>>();
            pawn_corr_hist = std::make_unique<MDArray<Score, 16384, 2>>();
            white_non_pawn_corr_hist = std::make_unique<MDArray<Score, 16384, 2>>();
            black_non_pawn_corr_hist = std::make_unique<MDArray<Score, 16384, 2>>();
            cont_corr_hist = std::make_unique<MDArray<Score, 1024, 1024>>();
            clear(); 
        };

        HistoryValue score(const BoardHistory& hist, Move move, Side stm) const;
        HistoryValue mainhist_score(Move move, Side stm) const { return main_hist[move.hist_idx(stm)]; };
        HistoryValue conthist_score(const BoardHistory& hist, Move move) const;
        HistoryValue capthist_score(const BoardHistory& hist, const Move move) const;
        Score corrhist_score(const Position& pos, const Score static_eval, const BoardHistory& hist) const;

        void update_scores(const BoardHistory& hist, std::span<const Move> moves, ScoredMove current_move, Side stm, int depth);
        void update_mainhist_score(Move move, Side stm, HistoryValue bonus);
        void update_conthist_score(const BoardHistory& hist, Move move, HistoryValue bonus);
        void update_capthist_score(const BoardHistory& hist, Move move, HistoryValue bonus);
        void update_corrhist_score(const Position& pos, const Score static_eval, const Score search_score, const int depth, const BoardHistory& hist);
        void clear() { 
            std::for_each(cont_hist->begin(), cont_hist->end(), [](auto& arr) { arr.fill(0); });
            std::for_each(capt_hist->begin(), capt_hist->end(), [](auto& arr) { arr.fill(0); });
            std::for_each(pawn_corr_hist->begin(), pawn_corr_hist->end(), [](auto& arr) { arr.fill(0); });
            std::for_each(white_non_pawn_corr_hist->begin(), white_non_pawn_corr_hist->end(), [](auto& arr) { arr.fill(0); });
            std::for_each(black_non_pawn_corr_hist->begin(), black_non_pawn_corr_hist->end(), [](auto& arr) { arr.fill(0); });
            std::for_each(cont_corr_hist->begin(), cont_corr_hist->end(), [](auto& arr) { arr.fill(0); });
            main_hist.fill(0); 
        };
};