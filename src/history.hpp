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
        std::array<HistoryValue, 1 << (1 + 6 + 6)> quiet_fromto_hist;
        std::array<HistoryValue, 6 * 64 * 2> quiet_pieceto_hist; 
        std::unique_ptr<MDArray<HistoryValue, 1024, 1024>> cont_hist;
        std::unique_ptr<MDArray<HistoryValue, 1024, 6>> capt_hist;
        std::unique_ptr<MDArray<Score, 16384, 2>> pawn_corr_hist;
        std::unique_ptr<MDArray<Score, 16384, 2>> white_non_pawn_corr_hist;
        std::unique_ptr<MDArray<Score, 16384, 2>> black_non_pawn_corr_hist;
        std::unique_ptr<MDArray<Score, 1024, 1024>> cont_corr_hist;

        static size_t calc_hist_idx(Move move, Side stm) { return move.hist_idx(stm); };
        static HistoryValue bonus(int depth) { return 16 * (depth + 1) * (depth + 1); };
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

        HistoryValue score(const Position& pos, const BoardHistory& hist, Move move, Side stm) const;
        auto mainhist_score(const Position& pos, const Move move, const Side stm) const -> HistoryValue {
            const auto score = quiet_fromto_hist[fromto_idx(move, stm)] + quiet_pieceto_hist[pieceto_idx(pos, move, stm)];
            return score / 2;
        };
        HistoryValue conthist_score(const BoardHistory& hist, Move move) const;
        HistoryValue capthist_score(const BoardHistory& hist, const Move move) const;
        Score corrhist_score(const Position& pos, const Score static_eval, const BoardHistory& hist) const;

        static auto fromto_idx(const Move move, const Side stm) -> usize { return static_cast<i32>(stm) << 12 | static_cast<i32>(move.dst_sq()) << 6 | static_cast<i32>(move.src_sq()); };
        static auto pieceto_idx(const Position& pos, const Move move, const Side stm) -> usize { return pos.piece_to(move) << 1 | static_cast<i32>(stm); };

        void update_scores(const Position& pos, const BoardHistory& hist, std::span<const Move> moves, ScoredMove current_move, Side stm, int depth);
        void update_mainhist_score(const Position& pos, const Move move, const Side stm, const HistoryValue bonus);
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
            quiet_fromto_hist.fill(0); 
            quiet_pieceto_hist.fill(0);
        };
};