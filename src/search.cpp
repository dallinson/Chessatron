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
                printf("%s: 1\n", moves[i].move.to_string().c_str());
            }
        }
        return moves.len();
    }

    for (size_t i = 0; i < moves.len(); i++) {
        uint64_t val;
        c.make_move(moves[i].move, m);
        val = perft<false>(c, m, depth - 1);
        if constexpr (print_debug) {
            std::cout << moves[i].move.to_string() << ": " << val << std::endl;
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
    return moves[rand() % moves.len()].move;
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
    return c.get_halfmove_clock() >= 100 || is_threefold_repetition(m, c.get_halfmove_clock(), c.get_zobrist_key()) || Search::detect_insufficient_material(c, c.get_side_to_move());
}

bool Search::static_exchange_evaluation(const ChessBoard& board, const Move move, const int threshold) {
    PieceTypes next_victim = move.is_promotion() ? move.get_promotion_piece_type() : board.get_piece(move.get_src_square()).get_type();

    Score balance = Search::SEEScores[static_cast<int>(board.get_piece(move.get_dest_square()).get_type())];
    if (move.is_promotion()) {
        balance += (Search::SEEScores[static_cast<int>(move.get_promotion_piece_type())] - Search::SEEScores[static_cast<int>(PieceTypes::PAWN)]);
    } else if (move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        balance = Search::SEEScores[static_cast<int>(PieceTypes::PAWN)];
    }

    balance -= threshold;

    if (balance < 0) return false;

    balance -= Search::SEEScores[static_cast<int>(next_victim)];

    if (balance >= 0) return true;

    const Bitboard bishops = board.get_bishop_occupancy() | board.get_queen_occupancy();
    const Bitboard rooks = board.get_rook_occupancy() | board.get_queen_occupancy();

    Bitboard occupied = board.get_occupancy();
    occupied ^= bit(move.get_src_square());
    occupied |= bit(move.get_dest_square());
    if (move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        const auto ep_target_square = get_position(move.get_src_rank(), move.get_dest_file());
        occupied ^= bit(ep_target_square);
    }

    Bitboard attackers = (MoveGenerator::get_attackers(board, board.get_side_to_move(), move.get_dest_square(), occupied)
                    | MoveGenerator::get_attackers(board, ENEMY_SIDE(board.get_side_to_move()), move.get_dest_square(), occupied))
                    & occupied;
    
    Side moving_side = ENEMY_SIDE(board.get_side_to_move());

    while (true) {
        const Bitboard this_side_attackers = attackers & board.get_occupancy(moving_side);

        if (this_side_attackers == 0) {
            break;
        }

        Bitboard victim_attackers = 0;

        for (next_victim = PieceTypes::PAWN; next_victim <= PieceTypes::QUEEN; next_victim = static_cast<PieceTypes>(static_cast<int>(next_victim) + 1)) {
            victim_attackers = this_side_attackers & board.get_bitboard((2 * (static_cast<int>(next_victim) - 1)) + static_cast<int>(moving_side));
            if (victim_attackers != 0) {
                break;
            }
        }

        occupied ^= (victim_attackers & -victim_attackers);

        if (next_victim == PieceTypes::PAWN || next_victim == PieceTypes::BISHOP || next_victim == PieceTypes::QUEEN) {
            // the pieces that attack diagonally
            attackers |= MoveGenerator::generate_bishop_movemask(occupied, move.get_dest_square()) & bishops;
        }

        if (next_victim == PieceTypes::ROOK || next_victim == PieceTypes::QUEEN) {
            // the pieces that attack orthogonally
            attackers |= MoveGenerator::generate_rook_movemask(occupied, move.get_dest_square()) & rooks;
        }

        attackers &= occupied;

        moving_side = ENEMY_SIDE(moving_side);

        balance = -balance - 1 - Search::SEEScores[static_cast<int>(next_victim)];

        if (balance >= 0) {
            if (next_victim == PieceTypes::KING && (attackers & board.get_occupancy(moving_side))) {
                moving_side = ENEMY_SIDE(moving_side);
            }
            break;
        }
    }

    return board.get_side_to_move() != moving_side;
}

bool Search::detect_insufficient_material(const ChessBoard& board, const Side side) {
    const Side enemy_side = ENEMY_SIDE(side);
    if (board.get_occupancy(enemy_side) == board.get_king_occupancy(enemy_side)) {
        // if the enemy side only has the king
        const Bitboard pieces = board.get_queen_occupancy(side) | board.get_rook_occupancy(side) | board.get_bishop_occupancy(side) | board.get_knight_occupancy(side) | board.get_pawn_occupancy(side);
        if (pieces == 0) {
            return true;
        }
        if (pieces == board.get_bishop_occupancy(side) || pieces == board.get_knight_occupancy(side)) {
            return std::popcount(pieces) == 1;
        }
    }
    return false;
}

template <NodeTypes node_type>
Score SearchHandler::negamax_step(Score alpha, Score beta, int depth, int ply, TranspositionTable& transpositions, uint64_t& node_count) {

    if (Search::is_draw(board, history)) {
        return 0;
    }

    constexpr auto pv_node_type = is_pv_node(node_type) ? NodeTypes::PV_NODE : NodeTypes::NON_PV_NODE;
    int extensions = 0;

    const auto tt_entry = table[board];
    if constexpr (!is_pv_node(node_type)) {
        const bool should_cutoff = tt_entry.get_key() == board.get_zobrist_key() 
                                   && tt_entry.get_depth() >= depth
                                   && (tt_entry.get_bound_type() == BoundTypes::EXACT_BOUND
                                   || (tt_entry.get_bound_type() == BoundTypes::LOWER_BOUND && tt_entry.get_score() >= beta)
                                   || (tt_entry.get_bound_type() == BoundTypes::UPPER_BOUND && tt_entry.get_score() <= alpha));
        if (should_cutoff) {
            return tt_entry.get_score();
        }
    }

    if (depth <= 0) {
        return quiescent_search<pv_node_type>(alpha, beta, ply, transpositions, node_count);
        // return c.evaluate();
    }

    if (board.get_checkers(board.get_side_to_move()) != 0) {
        extensions += 1;
    } 

    const auto static_eval = Evaluation::evaluate_board(board);
    if (ply >= MAX_PLY) {
        return static_eval;
    }

    // Reverse futility pruning
    if constexpr (!is_pv_node(node_type)) {
        if (board.get_checkers(board.get_side_to_move()) == 0 && depth < 7 && (static_eval - (70 * depth)) >= beta) {
            return beta;
        }
    }

    if (static_eval >= beta && board.get_checkers(board.get_side_to_move()) == 0 && (history.len() == 0 || (history[history.len() - 1].get_move() != Move::NULL_MOVE))) {
        // Try null move pruning if we aren't in check
        board.make_move(Move::NULL_MOVE, history);
        // First we make the null move
        auto null_score = -negamax_step<pv_node_type>(-beta, -alpha, depth - 1 - (depth >= 8 ? 3 : 2), ply + 1, transpositions, node_count);
        board.unmake_move(history);
        if (null_score >= beta) {
            return beta;
        }
    }

    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(board, board.get_side_to_move());
    if (moves.len() == 0) {
        if (board.get_checkers(board.get_side_to_move()) != 0) {
            // if in check
            return MagicNumbers::NegativeInfinity - depth;
        } else {
            return 0;
        }
    } else if (moves.len() == 1) {
        extensions += 1;
    }
    // mate and draw detection

    bool found_pv_move = false;
    MoveOrdering::reorder_moves(moves, board, tt_entry.get_key() == board.get_zobrist_key() ? tt_entry.get_pv_move() : Move::NULL_MOVE, found_pv_move, history_table);
    // move reordering

    if (depth >= 5 && !found_pv_move) {
        depth -= 1;
    }
    // iir

    Move best_move = Move::NULL_MOVE;
    search_stack[ply].quiet_alpha_raises.clear();
    Score best_score = MagicNumbers::NegativeInfinity;
    const Score original_alpha = alpha;
    for (size_t evaluated_moves = 0; evaluated_moves < moves.len(); evaluated_moves++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[evaluated_moves];

        if (depth <= 10 && !Search::static_exchange_evaluation(board, move.move, move.move.is_capture() ? (-20 * depth * depth) : (-65 * depth))) {
            continue;
        }

        board.make_move(move.move, history);
        node_count += 1;
        Score score;

        const bool is_lmr_potential_move = evaluated_moves >= 1 + static_cast<size_t>(is_pv_node(node_type));
        
        // See if we can perform LMR
        if (depth >= 2 && is_lmr_potential_move) {
            const auto lmr_reduction = static_cast<int>(std::round(1.30 + ((MagicNumbers::LnValues[depth] * MagicNumbers::LnValues[evaluated_moves]) / 2.80)));
            score = -negamax_step<NodeTypes::NON_PV_NODE>(-(alpha + 1), -alpha, depth - lmr_reduction + extensions, ply + 1, transpositions, node_count);

            // it's possible the LMR score will raise alpha; in this case we re-search with the full depth
            if (score > alpha) {
                score = -negamax_step<NodeTypes::NON_PV_NODE>(-(alpha + 1), -alpha, depth - 1 + extensions, ply + 1, transpositions, node_count);
            }
        }
        // if we didn't perform LMR
        else if (!is_pv_node(node_type) || is_lmr_potential_move) {
            score = -negamax_step<NodeTypes::NON_PV_NODE>(-(alpha + 1), -alpha, depth - 1 + extensions, ply + 1, transpositions, node_count);
        }

        if (is_pv_node(node_type) && (!is_lmr_potential_move || score > alpha)) {
            score = -negamax_step<NodeTypes::PV_NODE>(-beta, -alpha, depth - 1 + extensions, ply + 1, transpositions, node_count);
        }

        board.unmake_move(history);
        if (score > best_score) {
            best_score = score;
            best_move = move.move;
            if constexpr (node_type == NodeTypes::ROOT_NODE) {
                pv_move = best_move;
            }
        }
        if (score >= beta) {
            transpositions.store(TranspositionTableEntry(best_move, depth, BoundTypes::LOWER_BOUND, score, board.get_zobrist_key()), board);
            if (!move.move.is_capture()) {
                history_table[move.move.get_history_idx(board.get_side_to_move())] += (depth * depth);
                for (size_t i = 0; i < search_stack[ply].quiet_alpha_raises.len(); i++) {
                    history_table[search_stack[ply].quiet_alpha_raises[i].move.get_history_idx(board.get_side_to_move())] -= (depth * depth);
                }
            }
            return beta;
        }
        if (score > alpha) {
            alpha = score;
            if (!move.move.is_capture()) {
                search_stack[ply].quiet_alpha_raises.add_move(move.move);
            }
        }
    }
    const BoundTypes bound_type = alpha != original_alpha ? BoundTypes::EXACT_BOUND : BoundTypes::UPPER_BOUND;
    transpositions.store(TranspositionTableEntry(best_move, depth, bound_type, best_score, board.get_zobrist_key()), board);
    return alpha;
}

template <NodeTypes node_type>
Score SearchHandler::quiescent_search(Score alpha, Score beta, int ply, TranspositionTable& transpositions, uint64_t& node_count) {
    if (Search::is_draw(board, history)) {
        return 0;
    }
    Score static_eval = Evaluation::evaluate_board(board);
    if (ply >= MAX_PLY) {
        return static_eval;
    }
    if (static_eval >= beta) {
        return beta;
    }
    alpha = std::max(static_eval, alpha);

    MoveList moves;
    if (board.get_checkers(board.get_side_to_move()) == 0) {
        // if not in check
        moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIESCENCE>(board, board.get_side_to_move());
        if (moves.len() == 0 && MoveGenerator::generate_legal_moves<MoveGenType::NON_QUIESCENCE>(board, board.get_side_to_move()).len() == 0) {
            if (board.get_checkers(board.get_side_to_move()) != 0) {
                // if in check
                return MagicNumbers::NegativeInfinity;
            } else {
                return 0;
            }
        }
    } else {
        // If in check, generate all moves
        moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(board, board.get_side_to_move());
        if (moves.len() == 0) {
            return MagicNumbers::NegativeInfinity;
        }
    }

    bool found_pv_move = false;
    MoveOrdering::reorder_moves(moves, board, Move::NULL_MOVE, found_pv_move, history_table);
    int evaluated_moves = 0;
    for (size_t i = 0; i < moves.len(); i++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[i];

        if (move.move.is_capture()) {
            if (!move.see_ordering_result) {
                continue;
            }
        } else {
            if (!Search::static_exchange_evaluation(board, move.move, -65)) {
                continue;
            }
        }

        board.make_move(move.move, history);
        node_count += 1;
        Score score;
        if constexpr (is_pv_node(node_type)) {
            if (evaluated_moves == 0) {
                score = -quiescent_search<NodeTypes::PV_NODE>(-beta, -alpha, ply + 1, transpositions, node_count);
            } else {
                score = -quiescent_search<NodeTypes::NON_PV_NODE>(-alpha - 1, -alpha, ply + 1, transpositions, node_count);
                if (score > alpha) {
                    score = -quiescent_search<NodeTypes::PV_NODE>(-beta, -alpha, ply + 1, transpositions, node_count);
                }
            }
        } else {
            score = -quiescent_search<NodeTypes::NON_PV_NODE>(-alpha - 1, -alpha, ply + 1, transpositions, node_count);
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

Score SearchHandler::run_aspiration_window_search(int depth, Score previous_score) {
    Score window = 40;
    Score alpha, beta;
    while (true) {
        if (depth <= 4) {
            alpha = MagicNumbers::NegativeInfinity;
            beta = MagicNumbers::PositiveInfinity;
        } else {
            alpha = previous_score - window;
            beta = previous_score + window;
        }

        previous_score = negamax_step<NodeTypes::ROOT_NODE>(alpha, beta, depth, 0, table, node_count);

        if (search_cancelled) {
            return previous_score;
        }

        if (alpha < previous_score && previous_score < beta) {
            break;
        }

        window *= 2;
    }

    return previous_score;
}

Move SearchHandler::run_iterative_deepening_search() {
    node_count = 0;
    pv_move = Move::NULL_MOVE;
    // reset pv move so we don't accidentally play an illegal one from a previous search
    const auto search_start_point = std::chrono::steady_clock::now();
    // TranspositionTable transpositions;
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(board, board.get_side_to_move());
    // We generate legal moves only as it saves us having to continually rerun legality checks
    if (moves.len() == 1) {
        return moves[0].move;
        // If only one move is legal in this position we don't need to search; we can just return the one legal move
        // in order to save some time
    }

    history_table.fill(0);

    Score current_score = 0;
    for (int depth = 1; depth <= TimeManagement::get_search_depth(tc) && !search_cancelled; depth++) {
        
        current_score = run_aspiration_window_search(depth, current_score);
        const auto time_so_far = std::max(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - search_start_point).count(), (int64_t) 1);
        // Set time so far to a minimum of 1 to avoid divide by 0 in nps calculation

        if (!search_cancelled && print_info) {
            const auto nps = static_cast<uint64_t>(node_count / (static_cast<float>(time_so_far) / 1000));
            printf("info depth %d bestmove %s nodes %" PRIu64 " nps %" PRIu64 " ", depth, pv_move.to_string().c_str(), node_count, nps);
            if (std::abs(current_score) >= MagicNumbers::PositiveInfinity) {
                // If this is a checkmate
                printf("mate %d\n", ((current_score / std::abs(current_score)) * depth) / 2);
                // Divide by 2 as mate expects the result in moves, not plies
                if (current_score == MagicNumbers::PositiveInfinity) {
                    return pv_move;
                }
                // No need to check any other moves if we can guarantee a mate
            } else {
                printf("cp %d\n", current_score);
            }
        }

        if (TimeManagement::is_time_based_tc(tc) && time_so_far > TimeManagement::calculate_soft_limit(tc)) {
            break;
        }
    }
    return pv_move;
}