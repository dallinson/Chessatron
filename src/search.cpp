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

    moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, c.get_side_to_move());

    if (depth == 1) {
        if constexpr (print_debug) {
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
        if constexpr (print_debug) {
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
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, c.get_side_to_move());
    return moves[rand() % moves.len()];
}

bool Search::is_threefold_repetition(const MoveHistory& m, const int halfmove_clock, const ZobristKey z) {
    int counter = 1;
    const int mh_len = m.len();
    for (int i = mh_len - 1; i > 0 && i > mh_len - halfmove_clock; i--) {
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

template <bool pv_node>
Score SearchHandler::negamax_step(Score alpha, Score beta, int depth, TranspositionTable& transpositions, uint64_t& node_count) {

    if (Search::is_draw(board, history)) {
        return 0;
    }

    if (depth <= 0) {
        return quiescent_search<pv_node>(alpha, beta, transpositions, node_count);
        // return c.evaluate();
    }

    if (board.get_checkers(board.get_side_to_move()) == 0) {
        // Try null move pruning if we aren't in check
        board.make_move(Move::NULL_MOVE, history);
        // First we make the null move
        auto null_score = -negamax_step<pv_node>(-beta, -alpha, depth - 1 - 2, transpositions, node_count);
        board.unmake_move(history);
        if (null_score >= beta) {
            return beta;
        }
    }

    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(board, board.get_side_to_move());
    if (moves.len() == 0) {
        if (board.get_checkers(board.get_side_to_move()) != 0) {
            // if in check
            return MagicNumbers::NegativeInfinity;
        } else {
            return 0;
        }
    }
    // mate detection
    bool found_pv_move = MoveOrdering::reorder_pv_move(moves, table[board].get_pv_move());
    const auto capture_count = MoveOrdering::reorder_captures_first(moves, static_cast<size_t>(found_pv_move)) - static_cast<size_t>(found_pv_move);
    MoveOrdering::sort_captures_mvv_lva(moves, board, static_cast<size_t>(found_pv_move), capture_count);
    Move best_move = Move::NULL_MOVE;
    Score best_score = MagicNumbers::NegativeInfinity;
    int evaluated_moves = 0;
    for (size_t i = 0; i < moves.len(); i++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[i];
        board.make_move(move, history);
        node_count += 1;
        Score score;
        if constexpr (pv_node) {
            if (evaluated_moves == 0) {
                score = -negamax_step<true>(-beta, -alpha, depth - 1, transpositions, node_count);
            } else {
                score = -negamax_step<false>(-alpha - 1, -alpha, depth - 1, transpositions, node_count);
                if (score > alpha) {
                    score = -negamax_step<true>(-beta, -alpha, depth - 1, transpositions, node_count);
                }
            }
        } else {
            score = -negamax_step<false>(-alpha - 1, -alpha, depth - 1, transpositions, node_count);
        }
        board.unmake_move(history);
        evaluated_moves += 1;
        if (score >= beta) {
            return beta;
        }
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
        alpha = std::max(score, alpha);
    }
    transpositions[board] = TranspositionTableEntry(best_move);
    return alpha;
}

template <bool pv_node> Score SearchHandler::quiescent_search(Score alpha, Score beta, TranspositionTable& transpositions, uint64_t& node_count) {
    if (Search::is_draw(board, history)) {
        return 0;
    }
    Score stand_pat = Evaluation::evaluate_board(board);
    if (stand_pat >= beta) {
        return beta;
    }
    alpha = std::max(stand_pat, alpha);
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::CAPTURES>(board, board.get_side_to_move());
    if (moves.len() == 0 && MoveGenerator::generate_legal_moves<MoveGenType::NON_CAPTURES>(board, board.get_side_to_move()).len() == 0) {
        if (board.get_checkers(board.get_side_to_move()) != 0) {
            // if in check
            return MagicNumbers::NegativeInfinity;
        } else {
            return 0;
        }
    }
    auto capture_count = moves.len();
    MoveOrdering::sort_captures_mvv_lva(moves, board, 0, capture_count);
    int evaluated_moves = 0;
    for (size_t i = 0; i < capture_count; i++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[i];
        board.make_move(move, history);
        node_count += 1;
        Score score;
        if constexpr (pv_node) {
            if (evaluated_moves == 0) {
                score = -quiescent_search<true>(-beta, -alpha, transpositions, node_count);
            } else {
                score = -quiescent_search<false>(-alpha - 1, -alpha, transpositions, node_count);
                if (score > alpha) {
                    score = -quiescent_search<true>(-beta, -alpha, transpositions, node_count);
                }
            }
        } else {
            score = -quiescent_search<false>(-alpha - 1, -alpha, transpositions, node_count);
        }

        board.unmake_move(history);
        evaluated_moves += 1;
        if (score >= beta) {
            return beta;
        }
        alpha = std::max(score, alpha);
    }
    return alpha;
}

Move SearchHandler::run_iterative_deepening_search() {
    uint64_t node_count = 0;
    const auto search_start_point = std::chrono::steady_clock::now();
    Move best_move_so_far = 0;
    // TranspositionTable transpositions;
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(board, board.get_side_to_move());
    // We generate legal moves only as it saves us having to continually rerun legality checks
    if (moves.len() == 1) {
        return moves[0];
        // If only one move is legal in this position we don't need to search; we can just return the one legal move
        // in order to save some time
    }
    for (int depth = 1; depth <= perft_depth && !search_cancelled; depth++) {
        int best_score_this_depth = MagicNumbers::NegativeInfinity;
        Move best_move_this_depth = Move::NULL_MOVE;
        // We need to init for the depth=1 iteration where no PV-move exists
        Score alpha = MagicNumbers::NegativeInfinity;
        Score beta = MagicNumbers::PositiveInfinity;
        Score score = MagicNumbers::NegativeInfinity;

        const bool found_pv_move = MoveOrdering::reorder_pv_move(moves, best_move_so_far);
        const auto capture_count =
            MoveOrdering::reorder_captures_first(moves, static_cast<size_t>(found_pv_move)) - static_cast<size_t>(found_pv_move);
        MoveOrdering::sort_captures_mvv_lva(moves, board, static_cast<size_t>(found_pv_move), capture_count);

        int evaluated_moves = 0;
        for (size_t i = 0; i < moves.len(); i++) {
            board.make_move(moves[i], history);
            node_count += 1;
            if (evaluated_moves == 0) {
                score = -negamax_step<true>(-beta, -alpha, depth - 1, table, node_count);
            } else {
                score = -negamax_step<false>(-alpha - 1, -alpha, depth - 1, table, node_count);
                if (score > alpha) {
                    score = -negamax_step<true>(-beta, -alpha, depth - 1, table, node_count);
                }
            }
            board.unmake_move(history);
            evaluated_moves += 1;
            alpha = std::max(score, alpha);
            if (score > best_score_this_depth) {
                best_score_this_depth = score;
                best_move_this_depth = moves[i];
            }
            if (score >= beta) {
                break;
                // perform beta-cutoff
            }
        }

        const auto time_so_far = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - search_start_point).count();

        if (!search_cancelled) {
            const auto nps = static_cast<uint64_t>(node_count / (static_cast<float>(time_so_far) / 1000));
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

        if (time_so_far > ((search_time_ms * 3) / 10)) {
            break;
        }
    }
    return best_move_so_far;
}