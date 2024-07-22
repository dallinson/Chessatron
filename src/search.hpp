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
#include "tunable.hpp"

enum class NodeTypes {
    ROOT_NODE,
    PV_NODE,
    NON_PV_NODE
};
constexpr inline bool is_pv_node(NodeTypes n) { return n == NodeTypes::ROOT_NODE || n == NodeTypes::PV_NODE; };

constexpr static int PLY_OFFSET = 4;
constexpr static int MAX_PLY = 250 + PLY_OFFSET;
constexpr static int MATE_FOUND = MagicNumbers::PositiveInfinity - MAX_PLY;

namespace Perft {
    uint64_t run_perft(Position& c, int depth, bool print_debug = false);
}

namespace Search {
    inline std::array<TunableInt, 7> SEEScores = {
        TUNABLE_INT("see_empty_value", 0, -100, 100),
        TUNABLE_INT("see_pawn_value", 100, 0, 200),
        TUNABLE_INT("see_knight_value", 300, 100, 500),
        TUNABLE_INT("see_bishop_value", 300, 100, 500),
        TUNABLE_INT("see_rook_value", 500, 300, 700),
        TUNABLE_INT("see_queen_value", 900, 500, 1300),
        TUNABLE_INT("see_king_value", 0, -100, 100)
    };

    Move select_random_move(const Position& c);
    bool is_threefold_repetition(const BoardHistory& m, const int halfmove_clock, const ZobristKey z);
    bool is_draw(const Position& c, const BoardHistory& m);
    bool static_exchange_evaluation(const Position& pos, const Move move, const int threshold);
    bool detect_insufficient_material(const Position& pos, const Side side);
} // namespace Search

enum class BoundTypes : uint8_t {
    NONE = 0,
    LOWER_BOUND = 1,
    UPPER_BOUND = 2,
    EXACT_BOUND = 3,
};

inline std::array<std::array<int, MAX_TURN_MOVE_COUNT + 1>, MAX_PLY + 1> LmrTable;
inline std::array<std::array<int, MAX_TURN_MOVE_COUNT + 1>, MAX_PLY + 1> generate_lmr_table();
inline void recompute_table() { LmrTable = generate_lmr_table(); };

inline auto log_table_offset = TUNABLE_FLOAT_CALLBACK("lmr_table_offset", 0.39, 0.05, 0.95, 0.002, [](){ recompute_table(); });
inline auto log_table_divisor = TUNABLE_FLOAT_CALLBACK("lmr_table_divisor", 2.11, 1.0, 3.0, 0.002, [](){ recompute_table(); });

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

class TranspositionTableEntry {
    private:
        ZobristKey _key;
        Score _score;
        Score _static_eval;
        Move pv_move;
        uint8_t _depth;
        BoundTypes _bound;

        friend class TranspositionTable;
        void set_score(Score new_score) { this->_score = new_score; };
        void set_move(Move new_move) { this->pv_move = new_move; };
    public:
        TranspositionTableEntry() : _key(0), pv_move(Move::NULL_MOVE()), _depth(0), _bound(BoundTypes::NONE) {};
        TranspositionTableEntry(Move pv_move, uint8_t depth, BoundTypes bound, Score score, Score static_eval, ZobristKey key) : _key(key), _score(score), _static_eval(static_eval), pv_move(pv_move), _depth(depth), _bound(bound) {};

        Move move() const { return this->pv_move; };
        uint8_t depth() const { return this->_depth; };
        BoundTypes bound_type() const { return this->_bound; };
        Score score() const { return this->_score; };
        Score static_eval() const { return this->_static_eval; };
        ZobristKey key() const { return this->_key; };
};

class TranspositionTable {
    private:
        std::vector<TranspositionTableEntry> table;

    public:
        TranspositionTable() {
            this->resize(16);
        };
        uint64_t tt_index(const ZobristKey key) const { return static_cast<uint64_t>((static_cast<__uint128_t>(key) * static_cast<__uint128_t>(table.size())) >> 64); };
        const TranspositionTableEntry& operator[](const Position& key) const { return table[tt_index(key.zobrist_key())]; };
        void store(TranspositionTableEntry entry, const Position& key) {
            const auto tt_key = tt_index(key.zobrist_key());
            if (table[tt_key].key() != entry.key()
                || entry.bound_type() == BoundTypes::EXACT_BOUND
                || entry.depth() + 5 > table[tt_key].depth()) {
                if (entry.score() <= MagicNumbers::NegativeInfinity + MAX_PLY) {
                    // If we're being mated
                    entry.set_score(MagicNumbers::NegativeInfinity);
                } else if (entry.score() >= MagicNumbers::PositiveInfinity - MAX_PLY) {
                    // If we have mate
                    entry.set_score(MagicNumbers::PositiveInfinity);
                }
                if (entry.move().is_null_move()) {
                    entry.set_move(table[tt_key].move());
                }
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
        void prefetch(const ZobristKey key) const {
            __builtin_prefetch(&table[tt_index(key)]);
        }
};

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