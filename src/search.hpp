#pragma once

#include <atomic>
#include <cstdint>
#include <future>
#include <mutex>
#include <optional>
#include <semaphore>
#include <thread>

#include <cmath>

#include "chessboard.hpp"
#include "evaluation.hpp"
#include "time_management.hpp"

enum class NodeTypes {
    ROOT_NODE,
    PV_NODE,
    NON_PV_NODE
};
constexpr inline bool is_pv_node(NodeTypes n) { return n == NodeTypes::ROOT_NODE || n == NodeTypes::PV_NODE; };

namespace Perft {
    uint64_t run_perft(ChessBoard& c, int depth, bool print_debug = false);
}

namespace Search {
    Move select_random_move(const ChessBoard& c);
    bool is_threefold_repetition(const MoveHistory& m, const int halfmove_clock, const ZobristKey z);
    bool is_draw(const ChessBoard& c, const MoveHistory& m);
} // namespace Search

enum class BoundTypes : uint8_t {
    LOWER_BOUND = 1,
    UPPER_BOUND = 2,
    EXACT_BOUND = 3,
};

class TranspositionTableEntry {
    private:
        ZobristKey key;
        Score score;
        Move pv_move;
        uint8_t depth;
        BoundTypes bound;
        uint16_t padding;

    public:
        TranspositionTableEntry() : pv_move(Move::NULL_MOVE) {};
        TranspositionTableEntry(Move pv_move, uint8_t depth, BoundTypes bound, Score score, ZobristKey key) : key(key), score(score), pv_move(pv_move), depth(depth), bound(bound) {};

        Move get_pv_move() const { return this->pv_move; };
        uint8_t get_depth() const { return this->depth; };
        BoundTypes get_bound_type() const { return this->bound; };
        Score get_score() const { return this->score; };
        ZobristKey get_key() const { return this->key; };
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
        const TranspositionTableEntry& operator[](const ChessBoard& key) const { return table[key.get_zobrist_key() >> (64 - 20)]; };
        void store(const TranspositionTableEntry entry, const ChessBoard& key) {
            const auto tt_key = key.get_zobrist_key() >> (64 - 20);
            if (entry.get_depth() >= table[tt_key].get_depth()) {
                table[tt_key] = entry;
            }
        }
};

class SearchHandler {
    private:
        std::thread searchThread;
        std::binary_semaphore semaphore{0};
        std::mutex search_mutex;
        std::condition_variable cv;
        
        ChessBoard board;
        MoveHistory history;
        std::atomic<bool> in_search, search_cancelled, shutting_down, should_perft, infinite_search = false;
        TranspositionTable table;
        std::atomic<int> current_search_id = 0;
        std::future<void> cancelFuture;
        uint16_t perft_depth;
        TimeControlInfo tc;
        Move pv_move;
        uint64_t node_count;
        bool print_info = true;

        void search_thread_function();
        Score run_aspiration_window_search(int depth, Score previous_score);
        template <NodeTypes node_type> Score negamax_step(Score alpha, Score beta, int depth, TranspositionTable& transpositions, uint64_t& node_count);
        template <NodeTypes node_type> Score quiescent_search(Score alpha, Score beta, TranspositionTable& transpositions, uint64_t& node_count);
        Move run_negamax(int depth, TranspositionTable& transpositions);
        Move run_iterative_deepening_search();

    public:
        SearchHandler();

        bool is_searching() { return this->in_search; };
        int get_current_search_id() { return this->current_search_id; };
        ChessBoard& get_board() { return this->board; };
        MoveHistory& get_history() { return this->history; };

        void set_board(const ChessBoard& c) { this->board = c; };
        uint64_t get_node_count() { return node_count; };
        void set_print_info(bool print) { print_info = print; };
        void reset();

        void search(const TimeControlInfo& tc);
        void run_bench(uint16_t depth=8);
        void run_perft(uint16_t depth);

        void EndSearch() { search_cancelled = true; }

        void shutdown();
};