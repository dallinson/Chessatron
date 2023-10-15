#include "search.hpp"

#include "move_generator.hpp"

void SearchHandler::search_thread_function() {
    int this_search_id;
    while (true) {
        semaphore.acquire();
        this_search_id = current_search_id;
        if (this->shutting_down) {
            return;
        }
        search_cancelled = false;
        in_search = true;
        if (should_perft) {
            Perft::run_perft(c, perft_depth, true);
            should_perft = false;
        } else {
            auto move = run_iterative_deepening_search();
            if (move.is_null_move()) {
                // Unlikely, but possible!
                move = Search::select_random_move(c);
                // Just choose a random move
            }
            if (this_search_id == current_search_id) {
                printf("bestmove %s\n", move.to_string().c_str());
                fflush(stdout);
            }
            // We've had issues with stdout not being flushed in the past
        }
        in_search = false;
    }
}

void SearchHandler::shutdown() {
    this->shutting_down = true;
    this->search_cancelled = true;
    // If we're in a search, quit searching ASAP
    semaphore.release();
    this->searchThread.join();
}

SearchHandler::SearchHandler() { this->searchThread = std::thread(&SearchHandler::search_thread_function, this); }

void SearchHandler::search(int ms, int32_t max_depth) {
    current_search_id += 1;
    search_cancelled = true;
    perft_depth = max_depth;
    // cancel a search if performing one
    semaphore.release();
    // We then wake up the search thread
    int id_to_cancel = current_search_id;
    if (ms > 0) {
        cancelFuture = std::async(std::launch::async, [ms, this, id_to_cancel]() {
            std::this_thread::sleep_for(std::chrono::milliseconds{ms});
            if (this->get_current_search_id() == id_to_cancel) {
                // only cancel the current searcg
                this->EndSearch();
            }
        });
    }
}

void SearchHandler::run_perft(int depth) {
    search_cancelled = true;
    // cancel any existing search
    perft_depth = depth;
    should_perft = true;
    in_search = true;
    semaphore.release();
}

void SearchHandler::reset() {
    this->EndSearch();
    c = ChessBoard();
}