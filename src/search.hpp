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

constexpr auto default_see_pawn_value = 97;
constexpr auto default_see_knight_value = 292;
constexpr auto default_see_bishop_value = 281;
constexpr auto default_see_rook_value = 509;
constexpr auto default_see_queen_value = 920;

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
    TUNABLE_SPECIFIER std::array<Score, 7> SEEScores = { 0, default_see_pawn_value, default_see_knight_value, default_see_bishop_value, default_see_rook_value, default_see_queen_value, 0 };
    inline void update_see_values();

    #ifdef IS_TUNE
    TUNABLE_SPECIFIER TunableInt see_pawn_value = TUNABLE_INT_CALLBACK("see_pawn_value", default_see_pawn_value, 0, 200, 0.002, [](){ update_see_values(); });
    TUNABLE_SPECIFIER TunableInt see_knight_value = TUNABLE_INT_CALLBACK("see_knight_value", default_see_knight_value, 100, 500, 0.002, [](){ update_see_values(); });
    TUNABLE_SPECIFIER TunableInt see_bishop_value = TUNABLE_INT_CALLBACK("see_bishop_value", default_see_bishop_value, 100, 500, 0.002, [](){ update_see_values(); });
    TUNABLE_SPECIFIER TunableInt see_rook_value = TUNABLE_INT_CALLBACK("see_rook_value", default_see_rook_value, 300, 700, 0.002, [](){ update_see_values(); });
    TUNABLE_SPECIFIER TunableInt see_queen_value = TUNABLE_INT_CALLBACK("see_queen_value", default_see_queen_value, 700, 1100, 0.002, [](){ update_see_values(); });

    inline void update_see_values() {
        SEEScores[1] = see_pawn_value;
        SEEScores[2] = see_knight_value;
        SEEScores[3] = see_bishop_value;
        SEEScores[4] = see_rook_value;
        SEEScores[5] = see_queen_value;
    }
    #endif

    Move select_random_move(const Position& c);
    bool is_threefold_repetition(const BoardHistory& m, const int halfmove_clock, const ZobristKey z);
    bool is_draw(const Position& c, const BoardHistory& m);
    bool static_exchange_evaluation(const Position& pos, const Move move, const int threshold);
    bool detect_insufficient_material(const Position& pos, const Side side);
} // namespace Search

inline std::array<std::array<int, MAX_TURN_MOVE_COUNT + 1>, MAX_PLY + 1> LmrTable;
inline std::array<std::array<int, MAX_TURN_MOVE_COUNT + 1>, MAX_PLY + 1> generate_lmr_table();
inline void recompute_table() { LmrTable = generate_lmr_table(); };

TUNABLE_SPECIFIER auto log_table_offset = TUNABLE_FLOAT_CALLBACK("lmr_table_offset", 0.3274, 0.05, 0.95, 0.002, [](){ recompute_table(); });
TUNABLE_SPECIFIER auto log_table_divisor = TUNABLE_FLOAT_CALLBACK("lmr_table_divisor", 2.1816, 1.0, 3.0, 0.002, [](){ recompute_table(); });

inline std::array<std::array<int, MAX_TURN_MOVE_COUNT + 1>, MAX_PLY + 1> generate_lmr_table() {
    std::array<std::array<int, MAX_TURN_MOVE_COUNT + 1>, MAX_PLY + 1> to_return = {};
    for (int i = 0; i <= MAX_PLY; i++) {
        std::array<int, MAX_TURN_MOVE_COUNT + 1> data;
        for (int j = 0; j <= MAX_TURN_MOVE_COUNT; j++) {
            if (i == 0 || j == 0) {
                data[j] = 0;
            } else {
                data[j] = static_cast<int>(log_table_offset + std::log(i) * std::log(j) / log_table_divisor);
            }
        }
        to_return[i] = data;
    }
    return to_return;
}


struct SearchStackFrame {
    Move killer_move = Move::NULL_MOVE();
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