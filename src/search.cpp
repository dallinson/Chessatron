#include "search.hpp"

#include <cinttypes>
#include <iostream>
#include <limits>
#include <vector>

#include "move_generator.hpp"
#include "move_ordering.hpp"

template <bool print_debug> // this could just as easily be done as a parameter but this gives some practice with templates
uint64_t perft(ChessBoard& c, MoveHistory& m, int depth) {

    MoveList moves;
    uint64_t to_return = 0;

    moves = MoveGenerator::generate_legal_moves(c, c.get_side_to_move());

    if (depth == 1) {
        if (print_debug) {
            for (size_t i = 0; i < moves.len(); i++) {
                printf("%s: 1\n", moves[i].to_string().c_str());
            }
        }
        return moves.len();
    }

    for (size_t i = 0; i < moves.len(); i++) {
        uint64_t val;
        c.make_move(moves[i], m);
        val = perft<false>(c, m, depth - 1);
        if (print_debug) {
            std::cout << moves[i].to_string() << ": " << val << std::endl;
        }
        to_return += val;
        c.unmake_move(m);
    }
    return to_return;
}

uint64_t Perft::run_perft(ChessBoard& c, int depth, bool print_debug) {
    MoveHistory m;
    uint64_t nodes = 0;
    if (print_debug) {
        nodes = perft<true>(c, m, depth);
    } else {
        nodes = perft<false>(c, m, depth);
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

bool Search::is_threefold_repetition(const MoveHistory& m, const int halfmove_clock, const ZobristKey z) {
    int counter = 1;
    const int mh_len = m.len();
    for (int i = mh_len - 1; i > mh_len - halfmove_clock; i--) {
        if (m[i].get_zobrist_key() == z) {
            counter += 1;
            if (counter >= 3) {
                return true;
            }
        }
        if (m[i].get_move().is_castling_move()) {
            break;
        }
    }
    return false;
}

bool Search::is_draw(const ChessBoard& c, const MoveHistory& m) {
    return c.get_halfmove_clock() >= 100 || is_threefold_repetition(m, c.get_halfmove_clock(), c.get_zobrist_key());
}

Score SearchHandler::negamax_step(Score alpha, Score beta, int depth, TranspositionTable& transpositions, uint64_t& node_count) {

    if (Search::is_draw(c, m)) {
        return 0;
    }

    if (depth <= 0) {
        return quiescent_search(alpha, beta, transpositions, node_count);
        // return c.evaluate();
    }

    if (c.get_checkers(c.get_side_to_move()) == 0) {
        // Try null move pruning if we aren't in check
        c.make_move(Move::NULL_MOVE, m);
        // First we make the null move
        auto null_score = -negamax_step(-beta, -alpha, depth - 1 - 2, transpositions, node_count);
        c.unmake_move(m);
        if (null_score >= beta) {
            return beta;
        }
    }

    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    MoveOrdering::reorder_captures(moves, c);
    Move best_move = Move::NULL_MOVE;
    Move best_move_from_previous_search = Move::NULL_MOVE;
    Score best_score = MagicNumbers::NegativeInfinity;
    if (transpositions[c].get_pv_move() != Move::NULL_MOVE) {
        best_move_from_previous_search = transpositions[c].get_pv_move();
        // The first move we evaluate will _always_ be the best move
        if (best_move_from_previous_search != Move::NULL_MOVE && MoveGenerator::is_move_legal(c, best_move_from_previous_search) &&
            MoveGenerator::is_move_pseudolegal(c, best_move_from_previous_search)) {
            c.make_move(best_move_from_previous_search, m);
            best_score = -negamax_step(-beta, -alpha, depth - 1, transpositions, node_count);
            alpha = std::max(best_score, alpha);
            c.unmake_move(m);
            node_count += 1;
        }
    }
    best_move = best_move_from_previous_search;
    bool evaled_move = false;
    if (best_score < beta) {
        for (size_t i = 0; i < moves.len(); i++) {
            if (search_cancelled) {
                break;
            }
            const auto& move = moves[i];
            if (!MoveGenerator::is_move_legal(c, move) || move == best_move_from_previous_search) {
                // don't evaluate legal moves or the previous best move
                continue;
            }
            evaled_move = true;
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
        }
        if (!evaled_move) {
            if (c.get_checkers(c.get_side_to_move()) != 0) {
                // if in check
                return MagicNumbers::NegativeInfinity;
            } else {
                return 0;
            }
        }
    }
    transpositions[c] = TranspositionTableEntry(best_move);
    return alpha;
}

Score SearchHandler::quiescent_search(Score alpha, Score beta, TranspositionTable& transpositions, uint64_t& node_count) {
    if (Search::is_draw(c, m)) {
        return 0;
    }
    Score stand_pat = Evaluation::evaluate_board(c);
    if (stand_pat >= beta) {
        return beta;
    }
    alpha = std::max(stand_pat, alpha);
    auto moves = MoveGenerator::generate_pseudolegal_moves(c, c.get_side_to_move());
    auto capture_count = MoveOrdering::reorder_captures(moves, c);
    for (size_t i = 0; i < capture_count; i++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[i];
        if (!MoveGenerator::is_move_legal(c, move)) {
            // In a quiescent search we're only interested in (legal) captures
            // Move reordering ensures that captures are positioned first
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
    }
    return alpha;
}

Move SearchHandler::run_iterative_deepening_search() {
    Move best_move_so_far = 0;
    // TranspositionTable transpositions;
    auto moves = MoveGenerator::generate_legal_moves(c, c.get_side_to_move());
    // We generate legal moves only as it saves us having to continually rerun legality checks
    if (moves.len() == 1) {
        return moves[0];
        // If only one move is legal in this position we don't need to search; we can just return the one legal move
        // in order to save some time
    }
    MoveOrdering::reorder_captures(moves, c);
    for (int depth = 1; depth <= perft_depth && !search_cancelled; depth++) {
        int best_score_this_depth = MagicNumbers::NegativeInfinity;
        Move best_move_this_depth = Move::NULL_MOVE;
        // We need to init for the depth=1 iteration where no PV-move exists
        Score alpha = MagicNumbers::NegativeInfinity;
        Score beta = MagicNumbers::PositiveInfinity;
        Score score = MagicNumbers::NegativeInfinity;
        uint64_t node_count = 0;
        if (!best_move_so_far.is_null_move()) {
            c.make_move(best_move_so_far, m);
            score = -negamax_step(-beta, -alpha, depth - 1, table, node_count);
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
                score = -negamax_step(-beta, -alpha, depth - 1, table, node_count);
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
            auto nps = static_cast<uint64_t>(
                node_count /
                (static_cast<float>(
                     std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - search_start_point).count()) /
                 1000000));
            printf("info depth %d bestmove %s nodes %" PRIu64 " nps %" PRIu64 " ", depth, best_move_this_depth.to_string().c_str(), node_count, nps);
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