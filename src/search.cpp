#include "search.hpp"

#include <iostream>
#include <limits>
#include <vector>
#include <cinttypes>

#include "move_generator.hpp"
#include "move_ordering.hpp"

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

int32_t SearchHandler::negamax_step(int32_t alpha, int32_t beta, int depth, std::unordered_map<ChessBoard, Move>& transpositions, uint64_t& node_count) {
    if (depth <= 0) {
        return quiescent_search(alpha, beta, transpositions, node_count);
        //return c.evaluate();
    }
    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    MoveOrdering::reorder_captures(moves);
    Move best_move = 0;
    Move best_move_from_previous_search = 0;
    int32_t best_score = MagicNumbers::NegativeInfinity;
    if (transpositions.contains(c)) {
        best_move_from_previous_search = transpositions[c];
        // The first move we evaluate will _always_ be the best move
        c.make_move(best_move_from_previous_search, m);
        best_score = -negamax_step(-beta, -alpha, depth - 1, transpositions, node_count);
        alpha = std::max(best_score, alpha);
        c.unmake_move(m);
        node_count += 1;
    }
    best_move = best_move_from_previous_search;
    if (best_score < beta) {
        for (size_t i = 0; i < moves.len(); i++) {
            const auto& move = moves[i];
            if (!MoveGenerator::is_move_legal(c, move) || move == best_move_from_previous_search) {
                // don't evaluate legal moves or the previous best move
                continue;
            }
            c.make_move(move, m);
            auto score = -negamax_step(-beta, -alpha, depth - 1, transpositions, node_count);
            c.unmake_move(m);
            node_count += 1;
            if (score >= beta) {
                return beta;
            }
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
            alpha = std::max(score, alpha);
            if (search_cancelled) {
                break;
            }
        }
    }
    transpositions[c] = best_move;
    return alpha;
}

int32_t SearchHandler::quiescent_search(int32_t alpha, int32_t beta, std::unordered_map<ChessBoard, Move>& transpositions, uint64_t& node_count) {
    int32_t stand_pat = c.evaluate();
    if (stand_pat >= beta) {
        return beta;
    }
    alpha = std::max(stand_pat, alpha);
    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    MoveOrdering::reorder_captures(moves);
    for (size_t i = 0; i < moves.len(); i++) {
        const auto& move = moves[i];
        if (!(move.is_capture() && MoveGenerator::is_move_legal(c, move))) {
            // In a quiescent search we're only interested in (legal) captures
            continue;
        }
        c.make_move(move, m);
        auto score = -quiescent_search(-beta, -alpha, transpositions, node_count);
        c.unmake_move(m);
        node_count += 1;
        if (score >= beta) {
            return beta;
        }
        alpha = std::max(score, alpha);
        if (search_cancelled) {
            break;
        }
    }
    return alpha;
}

Move SearchHandler::run_iterative_deepening_search() {
    Move best_move_so_far = 0;
    std::unordered_map<ChessBoard, Move> transpositions;
    auto moves = MoveGenerator::generate_legal_moves(c, c.get_side_to_move());
    MoveOrdering::reorder_captures(moves);
    // We generate legal moves only as it saves us having to continually rerun legality checks
    for (int depth = 1; depth < perft_depth && !search_cancelled; depth++) {
        int best_score_this_depth = MagicNumbers::NegativeInfinity;
        Move best_move_this_depth;
        // We need to init for the depth=1 iteration where no PV-move exists
        int32_t alpha = MagicNumbers::NegativeInfinity;
        int32_t beta = MagicNumbers::PositiveInfinity;
        int32_t score = MagicNumbers::NegativeInfinity;
        uint64_t node_count = 0;
        if (!best_move_so_far.is_null_move()) {
            c.make_move(best_move_so_far, m);
            score = -negamax_step(-beta, -alpha, depth - 1, transpositions, node_count);
            c.unmake_move(m);
            node_count += 1;
            alpha = std::max(score, alpha);
            best_score_this_depth = score;
            best_move_this_depth = best_move_so_far;
        }

        if (score < beta) {
            // If score >= beta then we beta-prune every other move!
            for (size_t i = 0; i < moves.len(); i++) {
                if (moves[i] == best_move_so_far) {
                    // If it has already been examined
                    continue;
                }
                c.make_move(moves[i], m);
                score = -negamax_step(-beta, -alpha, depth - 1, transpositions, node_count);
                c.unmake_move(m);
                node_count += 1;
                alpha = std::max(score, alpha);
                if (score > best_score_this_depth) {
                    best_score_this_depth = score;
                    best_move_this_depth = moves[i];
                }
                if (score >= beta) {
                    break;
                }
            }
        }

        if (!search_cancelled) {
            printf("info depth %d bestmove %s nodes %" PRIu64 " ", depth, best_move_this_depth.to_string().c_str(), node_count);
            if (std::abs(best_score_this_depth) == MagicNumbers::PositiveInfinity) {
                // If this is a checkmate
                printf("mate %d\n", ((best_score_this_depth / std::abs(best_score_this_depth)) * depth) / 2);
                // Divide by 2 as mate expects the result in moves, not plies
                if (best_score_this_depth == MagicNumbers::PositiveInfinity) {
                    return best_move_this_depth;
                }
                // No need to check any other moves if we can guarantee a mate
            } else {
                printf("cp %d\n", best_score_this_depth);
            }
            best_move_so_far = best_move_this_depth;
        }
    }
    return best_move_so_far;
}