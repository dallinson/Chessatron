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
    bool is_threefold_repetition(const MoveHistory& m, const int halfmove_clock, const ZobristKey z);
    bool is_draw(const ChessBoard& c, const MoveHistory& m);
} // namespace Search

class TranspositionTableEntry {
    private:
        Move pv_move;

    public:
        TranspositionTableEntry() : pv_move(Move::NULL_MOVE){};
        TranspositionTableEntry(Move pv_move) : pv_move(pv_move){};

        Move get_pv_move() { return this->pv_move; };
};

class TranspositionTable {
    private:
        TranspositionTableEntry* table;

    public:
        TranspositionTable() {
            table = (TranspositionTableEntry*) calloc((16 * 1024 * 1024) / sizeof(TranspositionTableEntry), sizeof(TranspositionTableEntry));
        };
        TranspositionTable& operator=(const TranspositionTable& other) {
            if (this != &other) {
                if (table != nullptr) {
                    free(table);
                }
                table = (TranspositionTableEntry*) calloc((16 * 1024 * 1024) / sizeof(TranspositionTableEntry), sizeof(TranspositionTableEntry));
                std::copy(other.table, other.table + ((16 * 1024 * 1024) / sizeof(TranspositionTableEntry)), table);
            }
            return *this;
        }
        ~TranspositionTable() { free(table); };
        TranspositionTableEntry& operator[](const ChessBoard& key) { return table[key.get_zobrist_key() >> (64 - 22)]; };
};

class SearchHandler {
    private:
        std::thread searchThread;
        std::binary_semaphore semaphore{0};
        ChessBoard board;
        MoveHistory history;
        std::atomic<bool> in_search, search_cancelled, shutting_down, should_perft = false;
        TranspositionTable table;
        std::atomic<int> current_search_id = 0;
        std::future<void> cancelFuture;
        int perft_depth = 0;
        uint32_t search_time_ms;
        Move bestMove;

        void search_thread_function();
        Score negamax_step(Score alpha, Score beta, int depth, TranspositionTable& transpositions, uint64_t& node_count);
        Score quiescent_search(Score alpha, Score beta, TranspositionTable& transpositions, uint64_t& node_count);
        Move run_negamax(int depth, TranspositionTable& transpositions);
        Move run_iterative_deepening_search();

    public:
        SearchHandler();

        bool is_searching() { return this->in_search; };
        int get_current_search_id() { return this->current_search_id; };
        ChessBoard& get_board() { return this->board; };
        MoveHistory& get_history() { return this->history; };

        void set_board(const ChessBoard& c) { this->board = c; };
        void reset();

        void search(int ms, int32_t max_depth = MagicNumbers::PositiveInfinity);

        void run_perft(int depth);

        void EndSearch() { search_cancelled = true; }

        void shutdown();
};