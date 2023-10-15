#include "search.hpp"

#include <iostream>
#include <limits>
#include <vector>

#include "move_generator.hpp"

template <bool print_debug> // this could just as easily be done as a parameter but this gives some practice with templates
uint64_t perft(ChessBoard& c, MoveHistory& m, int depth, std::vector<std::unordered_map<ChessBoard, uint64_t>>& cache_vec) {

    if (cache_vec[depth - 1].contains(c)) {
        return cache_vec[depth - 1].at(c);
    }

    MoveList moves;
    uint64_t to_return = 0;

    moves = MoveGenerator::generate_legal_moves(c, c.get_side_to_move());

    if (depth == 1) {
        if (print_debug) [[unlikely]] {
            for (size_t i = 0; i < moves.len(); i++) {
                printf("%s: 1\n", moves[i].to_string().c_str());
            }
        }
        return moves.len();
    }

    for (size_t i = 0; i < moves.len(); i++) {
        uint64_t val;
        c.make_move(moves[i], m);
        val = perft<false>(c, m, depth - 1, cache_vec);
        if (print_debug) {
            std::cout << moves[i].to_string() << ": " << val << std::endl;
        }
        to_return += val;
        c.unmake_move(m);
    }

    cache_vec[depth - 1].try_emplace(c, to_return);
    return to_return;
}

uint64_t Perft::run_perft(ChessBoard& c, int depth, bool print_debug) {
    MoveHistory m;
    std::vector<std::unordered_map<ChessBoard, uint64_t>> cache_vec;
    cache_vec.resize(depth);
    uint64_t nodes = 0;
    if (print_debug) {
        nodes = perft<true>(c, m, depth, cache_vec);
    } else {
        nodes = perft<false>(c, m, depth, cache_vec);
    }
    if (print_debug) {
        std::cout << std::endl << "Nodes searched: " << nodes << std::endl;
    }
    return nodes;
}

Move Search::select_random_move(const ChessBoard& c) {
    auto moves = MoveGenerator::generate_legal_moves(c, c.get_side_to_move());
    return moves[rand() % moves.len()];
}

int32_t SearchHandler::negamax_step(int32_t alpha, int32_t beta, int depth) {
    if (depth <= 0) {
        return c.get_score(c.get_side_to_move()) - c.get_score(ENEMY_SIDE(c.get_side_to_move()));
    }
    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    bool had_move = false;
    for (size_t i = 0; i < moves.len(); i++) {
        const auto& move = moves[i];
        if (!MoveGenerator::is_move_legal(c, move)) {
            continue;
        }
        had_move = true;
        c.make_move(move, m);
        auto score = -negamax_step(-beta, -alpha, depth - 1);
        c.unmake_move(m);
        if (score >= beta) {
            return beta;
        }
        alpha = std::max(score, alpha);
        if (search_cancelled) {
            break;
        }
    }
    return had_move ? alpha : MagicNumbers::NegativeInfinity;
}

int32_t SearchHandler::quiescent_search(int32_t alpha, int32_t beta) {
    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    bool had_move = false;
    for (size_t i = 0; i < moves.len(); i++) {
        const auto& move = moves[i];
        if (!(move.is_capture() && MoveGenerator::is_move_legal(c, move))) {
            // In a quiescent search we're only interested in (legal) captures
            continue;
        }
        had_move = true;
        c.make_move(move, m);
        auto score = -quiescent_search(-beta, -alpha);
        c.unmake_move(m);
        if (score >= beta) {
            return beta;
        }
        alpha = std::max(score, alpha);
        if (search_cancelled) {
            break;
        }
    }
    if (!had_move) {
        return c.evaluate();
    } else {
        return alpha;
    }
}

Move SearchHandler::run_negamax(int depth) {
    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    Move best_move = 0;
    int best_score = MagicNumbers::NegativeInfinity;
    int32_t alpha = MagicNumbers::NegativeInfinity;
    int32_t beta = MagicNumbers::PositiveInfinity;
    for (size_t i = 0; i < moves.len(); i++) {
        const auto& move = moves[i];
        if (!MoveGenerator::is_move_legal(c, move)) {
            continue;
        }
        c.make_move(move, m);
        int32_t score = -negamax_step(-beta, -alpha, depth - 1);
        // the next step gets negative infinity as the first arg and positive infinity as the second
        if (score > best_score) {
            best_move = move;
            best_score = score;
        }
        alpha = std::max(alpha, score);
        c.unmake_move(m);
        if (search_cancelled) {
            // checking here ensures we always find one move
            break;
        }
    }
    return best_move;
}

Move SearchHandler::run_iterative_deepening_search() {
    Move best_move = 0;
    for (int i = 1; i < perft_depth && !search_cancelled; i++) {
        auto result = run_negamax(i);
        printf("info depth %d bestmove %s\n", i, result.to_string().c_str());
        if (!search_cancelled) {
            best_move = result;
        }
    }
    return best_move;
}