#pragma once

#include <atomic>
#include <cstdint>
#include <future>
#include <mutex>
#include <optional>
#include <semaphore>
#include <thread>

#include "chessboard.hpp"

namespace Perft {
    uint64_t run_perft(ChessBoard& c, int depth, bool print_debug = false);
}

namespace Search {
    Move select_random_move(const ChessBoard& c);
} // namespace Search

class SearchHandler {
    private:
        std::thread searchThread;
        std::binary_semaphore semaphore{0};
        ChessBoard c;
        MoveHistory m;
        std::atomic<bool> isSearching, searchCancelled, shuttingDown, should_perft = false;
        std::atomic<int> current_search_id = 0;
        std::future<void> cancelFuture;
        int perft_depth = 0;
        Move bestMove;

        void search_thread_function();
        int32_t negamax_step(ChessBoard& c, MoveHistory& m, int depth);
        Move run_negamax(ChessBoard& c, MoveHistory& m, int depth = 4);

    public:
        SearchHandler();

        bool is_searching() { return this->isSearching; };
        int get_current_search_id() { return this->current_search_id; };

        void set_board(const ChessBoard& c) {
            this->c = c;
        };
        
        void reset();

        void search(int ms = 100);

        void run_perft(int depth);

        void EndSearch() { searchCancelled = true; }

        void shutdown();
};