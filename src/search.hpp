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
#include "history.hpp"
#include "time_management.hpp"
#include "ttable.hpp"
#include "tunable.hpp"
#include "tunable_definitions.hpp"

constexpr auto LMR_QUANT_CONSTANT = 1024;

enum class NodeTypes {
    ROOT_NODE,
    PV_NODE,
    NON_PV_NODE
};
constexpr inline bool is_pv_node(NodeTypes n) { return n == NodeTypes::ROOT_NODE || n == NodeTypes::PV_NODE; };

namespace Perft {
    uint64_t run_perft(Position& c, int depth, bool print_debug = false);
}

namespace Search {

    Move select_random_move(const Position& c);
    bool is_threefold_repetition(const BoardHistory& m, const int halfmove_clock, const ZobristKey z);
    bool is_draw(const Position& c, const BoardHistory& m);
    bool static_exchange_evaluation(const Position& pos, const Move move, const int threshold);
    bool detect_insufficient_material(const Position& pos, const Side side);
} // namespace Search

inline MDArray<i32, MAX_PLY + 1, MAX_TURN_MOVE_COUNT + 1> LmrTable;
inline MDArray<i32, MAX_PLY + 1, MAX_TURN_MOVE_COUNT + 1> generate_lmr_table();
inline void recompute_table() { LmrTable = generate_lmr_table(); };

TUNABLE_SPECIFIER auto log_table_offset = TUNABLE_FLOAT_CALLBACK("lmr_table_offset", 0.3290, 0.05, 0.95, 0.002, [](){ recompute_table(); });
TUNABLE_SPECIFIER auto log_table_divisor = TUNABLE_FLOAT_CALLBACK("lmr_table_divisor", 2.1977, 1.0, 3.0, 0.002, [](){ recompute_table(); });

inline MDArray<i32, MAX_PLY + 1, MAX_TURN_MOVE_COUNT + 1> generate_lmr_table() {
    MDArray<i32, MAX_PLY + 1, MAX_TURN_MOVE_COUNT + 1> to_return = {};
    for (int i = 0; i <= MAX_PLY; i++) {
        std::array<i32, MAX_TURN_MOVE_COUNT + 1> data;
        for (int j = 0; j <= MAX_TURN_MOVE_COUNT; j++) {
            if (i == 0 || j == 0) {
                data[j] = 0;
            } else {
                data[j] = static_cast<int>(LMR_QUANT_CONSTANT * (log_table_offset + std::log(i) * std::log(j) / log_table_divisor));
            }
        }
        to_return[i] = data;
    }
    return to_return;
}


struct SearchStackFrame {
    Move killer_move = Move::NULL_MOVE();
    Move excluded_move = Move::NULL_MOVE();
};

struct PvTable {
    std::array<int, MAX_PLY + 1> pv_length;
    std::array<std::array<Move, MAX_PLY + 1>, MAX_PLY + 1> pv_array;
};

class SearchHandler {
    private:
        std::thread search_thread;
        std::binary_semaphore semaphore{0};
        std::mutex search_mutex;
        std::condition_variable cv;
        
        BoardHistory board_hist;
        HistoryTable history_table;
        std::array<uint64_t, 4096> node_spent_table;
        std::array<SearchStackFrame, MAX_PLY + 2> search_stack;
        PvTable pv_table;

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
        template <NodeTypes node_type> Score negamax_step(const Position& pos, Score alpha, Score beta, int depth, int ply, uint64_t& node_count, bool is_cut_node);
        template <NodeTypes node_type> Score quiescent_search(const Position& pos, Score alpha, Score beta, int ply, uint64_t& node_count);
        Move run_iterative_deepening_search();

    public:
        SearchHandler();
        ~SearchHandler() { this->shutdown(); };

        bool is_searching() { return this->in_search; };
        int get_current_search_id() { return this->current_search_id; };
        Position& get_pos() { return this->board_hist[board_hist.len() - 1]; };
        BoardHistory& get_history() { return this->board_hist; };

        void set_pos(const Position& c) { 
            this->board_hist = BoardHistory(c);
        };
        void set_history(const BoardHistory& h) {
            this->board_hist = h;
        }
        uint64_t get_node_count() { return node_count; };
        void set_print_info(bool print) { print_info = print; };
        void reset();

        void search(const TimeControlInfo& tc);
        void run_bench(uint16_t depth=14);
        void run_perft(uint16_t depth);

        void EndSearch() { search_cancelled = true; }

        void shutdown();
};