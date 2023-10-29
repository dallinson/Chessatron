#pragma once

#include <atomic>
#include <cstdint>
#include <future>
#include <mutex>
#include <optional>
#include <semaphore>
#include <thread>

#include "chessboard.hpp"
#include "evaluation.hpp"

namespace Perft {
    uint64_t run_perft(ChessBoard& c, int depth, bool print_debug = false);
}

namespace Search {
    Move select_random_move(const ChessBoard& c);
} // namespace Search

class TranspositionTableEntry {
    private:
        int depth;
        Move pv_move;

    public:
        TranspositionTableEntry() : depth(0), pv_move(0){};
        TranspositionTableEntry(int depth, Move pv_move) : depth(depth), pv_move(pv_move){};

        int get_depth() { return this->depth; };
        Move get_pv_move() { return this->pv_move; };
};

class TranspositionTable {
    private:
        std::unordered_map<ChessBoard, TranspositionTableEntry> table;

    public:
        bool contains(const ChessBoard& key) const { return table.contains(key); };
        TranspositionTableEntry& operator[](const ChessBoard& key) { return table[key]; };
};

class SearchHandler {
    private:
        std::thread searchThread;
        std::binary_semaphore semaphore{0};
        ChessBoard c;
        MoveHistory m;
        std::atomic<bool> in_search, search_cancelled, shutting_down, should_perft = false;
        TranspositionTable table;
        std::atomic<int> current_search_id = 0;
        std::future<void> cancelFuture;
        int perft_depth = 0;
        Move bestMove;
        std::chrono::steady_clock::time_point search_start_point;

        void search_thread_function();
        Score negamax_step(Score alpha, Score beta, int depth, TranspositionTable& transpositions, uint64_t& node_count);
        Score quiescent_search(Score alpha, Score beta, TranspositionTable& transpositions, uint64_t& node_count);
        Move run_negamax(int depth, TranspositionTable& transpositions);
        Move run_iterative_deepening_search();

    public:
        SearchHandler();

        bool is_searching() { return this->in_search; };
        int get_current_search_id() { return this->current_search_id; };
        ChessBoard& get_board() { return this->c; };
        MoveHistory& get_history() { return this-> m; };

        void set_board(const ChessBoard& c) { this->c = c; };
        void reset();

        void search(int ms, int32_t max_depth = MagicNumbers::PositiveInfinity);

        void run_perft(int depth);

        void EndSearch() { search_cancelled = true; }

        void shutdown();
};