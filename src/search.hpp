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

constexpr static int MAX_PLY = 250;

namespace Perft {
    uint64_t run_perft(ChessBoard& c, int depth, bool print_debug = false);
}

namespace Search {
    constexpr std::array<Score, 7> SEEScores = { 0, 100, 300, 300, 500, 900, 0 };

    Move select_random_move(const ChessBoard& c);
    bool is_threefold_repetition(const MoveHistory& m, const int halfmove_clock, const ZobristKey z);
    bool is_draw(const ChessBoard& c, const MoveHistory& m);
    bool static_exchange_evaluation(const ChessBoard& board, const Move move, const int threshold);
    bool detect_insufficient_material(const ChessBoard& board, const Side side);
} // namespace Search

enum class BoundTypes : uint8_t {
    NONE = 0,
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
        TranspositionTableEntry() : key(0), pv_move(Move::NULL_MOVE), depth(0), bound(BoundTypes::NONE) {};
        TranspositionTableEntry(Move pv_move, uint8_t depth, BoundTypes bound, Score score, ZobristKey key) : key(key), score(score), pv_move(pv_move), depth(depth), bound(bound) {};

        Move get_pv_move() const { return this->pv_move; };
        uint8_t get_depth() const { return this->depth; };
        BoundTypes get_bound_type() const { return this->bound; };
        Score get_score() const { return this->score; };
        ZobristKey get_key() const { return this->key; };
};

class TranspositionTable {
    private:
        std::vector<TranspositionTableEntry> table;

    public:
        TranspositionTable() {
            this->resize(16);
        };
        uint64_t tt_index(const ZobristKey key) const { return static_cast<uint64_t>((static_cast<__uint128_t>(key) * static_cast<__uint128_t>(table.size())) >> 64); };
        const TranspositionTableEntry& operator[](const ChessBoard& key) const { return table[tt_index(key.get_zobrist_key())]; };
        void store(const TranspositionTableEntry entry, const ChessBoard& key) {
            const auto tt_key = tt_index(key.get_zobrist_key());
            if (entry.get_depth() >= table[tt_key].get_depth()) {
                table[tt_key] = entry;
            }
        }
        void clear() {
            std::fill(table.begin(), table.end(), TranspositionTableEntry());
        }
        void resize(size_t mb_size) {
            table.resize((mb_size * 1024 * 1024) / sizeof(TranspositionTableEntry));
            clear();
        }
};

struct SearchStackFrame {
    MoveList quiet_alpha_raises;
    Move killer_move;
};

class SearchHandler {
    private:
        std::thread searchThread;
        std::binary_semaphore semaphore{0};
        std::mutex search_mutex;
        std::condition_variable cv;
        
        ChessBoard board;
        MoveHistory history;
        std::array<int32_t, 8192> history_table;
        std::array<uint64_t, 4096> node_spent_table;
        std::array<SearchStackFrame, MAX_PLY + 2> search_stack;

        std::atomic<bool> in_search, search_cancelled, shutting_down, should_perft, infinite_search = false;
        std::atomic<int> current_search_id = 0;
        std::future<void> cancelFuture;
        uint16_t perft_depth;
        TimeControlInfo tc;
        Move pv_move;
        uint64_t node_count;
        bool print_info = true;

        void search_thread_function();
        Score run_aspiration_window_search(int depth, Score previous_score);
        template <NodeTypes node_type> Score negamax_step(Score alpha, Score beta, int depth, int ply, uint64_t& node_count);
        template <NodeTypes node_type> Score quiescent_search(Score alpha, Score beta, int ply, uint64_t& node_count);
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
        void run_bench(uint16_t depth=14);
        void run_perft(uint16_t depth);

        void EndSearch() { search_cancelled = true; }

        void shutdown();
};