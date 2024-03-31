#include "search.hpp"

#include <cinttypes>
#include <iostream>
#include <limits>
#include <vector>

#include "common.hpp"
#include "move_generator.hpp"
#include "move_ordering.hpp"

TranspositionTable tt;

template <bool print_debug> // this could just as easily be done as a parameter but this gives some practice with templates
uint64_t perft(const ChessBoard& old_board, BoardHistory& history, int depth) {

    MoveList moves;
    uint64_t to_return = 0;

    moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(old_board, old_board.get_side_to_move());

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
        auto& board = old_board.make_move(moves[i].move, history);
        val = perft<false>(board, history, depth - 1);
        if constexpr (print_debug) {
            std::cout << moves[i].move.to_string() << ": " << val << std::endl;
        }
        to_return += val;
        history.pop_board();
    }
    return to_return;
}

uint64_t Perft::run_perft(ChessBoard& board, int depth, bool print_debug) {
    BoardHistory history(board);
    uint64_t nodes = 0;
    const auto perft_start_point = std::chrono::steady_clock::now();
    if (print_debug) {
        nodes = perft<true>(board, history, depth);
    } else {
        nodes = perft<false>(board, history, depth);
    }
    if (print_debug) {
        const auto perft_time = std::max(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - perft_start_point).count(), (int64_t) 1);
        std::cout << std::endl << "Nodes searched: " << nodes << std::endl;
        std::cout << "NPS: " << static_cast<uint64_t>(nodes / (static_cast<float>(perft_time) / 1000)) << std::endl;
    }
    return nodes;
}

Move Search::select_random_move(const ChessBoard& c) {
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, c.get_side_to_move());
    return moves[rand() % moves.len()].move;
}

bool Search::is_threefold_repetition(const BoardHistory& history, const int halfmove_clock, const ZobristKey z) {
    int counter = 1;
    const int history_len = history.len();
    const auto castling_rights = history[history_len - 1].get_castling();
    for (int i = history_len - 2; i > 0 && i > history_len - halfmove_clock - 1; i--) {
        if (history[i].get_zobrist_key() == z) {
            counter += 1;
            if (counter >= 3) {
                return true;
            }
        }
        if (history[i].get_castling() != castling_rights) {
            break;
        }
    }
    return false;
}

bool Search::is_draw(const ChessBoard& board, const BoardHistory& history) {
    return board.get_halfmove_clock() > 100 || is_threefold_repetition(history, board.get_halfmove_clock(), board.get_zobrist_key())
           || Search::detect_insufficient_material(board, board.get_side_to_move());
}

bool Search::static_exchange_evaluation(const ChessBoard& board, const Move move, const int threshold) {
    PieceTypes next_victim = move.is_promotion() ? move.get_promotion_piece_type() : board.piece_at(move.src_sq()).get_type();

    Score balance = Search::SEEScores[static_cast<int>(board.piece_at(move.dst_sq()).get_type())];
    if (move.is_promotion()) {
        balance += (Search::SEEScores[static_cast<int>(move.get_promotion_piece_type())] - Search::SEEScores[static_cast<int>(PieceTypes::PAWN)]);
    } else if (move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        balance = Search::SEEScores[static_cast<int>(PieceTypes::PAWN)];
    }

    balance -= threshold;

    if (balance < 0)
        return false;

    balance -= Search::SEEScores[static_cast<int>(next_victim)];

    if (balance >= 0)
        return true;

    const Bitboard bishops = board.bishops() | board.queens();
    const Bitboard rooks = board.rooks() | board.queens();

    Bitboard occupied = board.occupancy();
    occupied ^= bit(move.src_sq());
    occupied |= bit(move.dst_sq());
    if (move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        const auto ep_target = get_position(move.src_rnk(), move.dst_fle());
        occupied ^= bit(ep_target);
    }

    Bitboard attackers = (MoveGenerator::get_attackers(board, board.get_side_to_move(), move.dst_sq(), occupied)
                          | MoveGenerator::get_attackers(board, enemy_side(board.get_side_to_move()), move.dst_sq(), occupied))
                         & occupied;

    Side moving_side = enemy_side(board.get_side_to_move());

    while (true) {
        const Bitboard this_side_attackers = attackers & board.occupancy(moving_side);

        if (this_side_attackers == 0) {
            break;
        }

        Bitboard victim_attackers = 0;

        for (next_victim = PieceTypes::PAWN; next_victim <= PieceTypes::QUEEN;
             next_victim = static_cast<PieceTypes>(static_cast<int>(next_victim) + 1)) {
            victim_attackers = this_side_attackers & board.get_bb(static_cast<int>(next_victim) - 1, static_cast<int>(moving_side));
            if (victim_attackers != 0) {
                break;
            }
        }

        occupied ^= (victim_attackers & -victim_attackers);

        if (next_victim == PieceTypes::PAWN || next_victim == PieceTypes::BISHOP || next_victim == PieceTypes::QUEEN) {
            // the pieces that attack diagonally
            attackers |= MoveGenerator::generate_bishop_mm(occupied, move.dst_sq()) & bishops;
        }

        if (next_victim == PieceTypes::ROOK || next_victim == PieceTypes::QUEEN) {
            // the pieces that attack orthogonally
            attackers |= MoveGenerator::generate_rook_mm(occupied, move.dst_sq()) & rooks;
        }

        attackers &= occupied;

        moving_side = enemy_side(moving_side);

        balance = -balance - 1 - Search::SEEScores[static_cast<int>(next_victim)];

        if (balance >= 0) {
            if (next_victim == PieceTypes::KING && (attackers & board.occupancy(moving_side))) {
                moving_side = enemy_side(moving_side);
            }
            break;
        }
    }

    return board.get_side_to_move() != moving_side;
}

bool Search::detect_insufficient_material(const ChessBoard& board, const Side side) {
    const Side enemy = enemy_side(side);
    if (board.occupancy(enemy) == board.kings(enemy)) {
        // if the enemy side only has the king
        const Bitboard pieces = board.queens(side) | board.rooks(side) | board.bishops(side) | board.knights(side) | board.pawns(side);
        if (pieces == 0) {
            return true;
        }
        if (pieces == board.bishops(side) || pieces == board.knights(side)) {
            return std::popcount(pieces) == 1;
        }
    }
    return false;
}

template <NodeTypes node_type>
Score SearchHandler::negamax_step(const ChessBoard& old_board, Score alpha, Score beta, int depth, int ply, uint64_t& node_count, bool is_cut_node) {

    pv_table.pv_length[ply] = ply;
    if (Search::is_draw(old_board, history)) {
        return 0;
    }

    constexpr auto pv_node_type = is_pv_node(node_type) ? NodeTypes::PV_NODE : NodeTypes::NON_PV_NODE;
    const auto child_cutnode_type = is_pv_node(node_type) ? true : !is_cut_node;
    int extensions = 0;

    const auto tt_entry = tt[old_board];
    if constexpr (!is_pv_node(node_type)) {
        const bool should_cutoff =
            tt_entry.key() == old_board.get_zobrist_key() && tt_entry.depth() >= depth
            && (tt_entry.bound_type() == BoundTypes::EXACT_BOUND || (tt_entry.bound_type() == BoundTypes::LOWER_BOUND && tt_entry.score() >= beta)
                || (tt_entry.bound_type() == BoundTypes::UPPER_BOUND && tt_entry.score() <= alpha));
        if (should_cutoff) {
            if (tt_entry.score() == MagicNumbers::PositiveInfinity) {
                return MagicNumbers::PositiveInfinity - ply;
            } else if (tt_entry.score() == MagicNumbers::NegativeInfinity) {
                return MagicNumbers::NegativeInfinity + ply;
            }
            return tt_entry.score();
        }
    }
    const bool tt_hit = tt_entry.key() == old_board.get_zobrist_key();

    if (depth <= 0) {
        return quiescent_search<pv_node_type>(old_board, alpha, beta, ply, node_count);
        // return c.evaluate();
    }

    if (old_board.in_check()) {
        extensions += 1;
    }

    const auto static_eval = tt_hit ? tt_entry.score() : Evaluation::evaluate_board(old_board);
    if (ply >= MAX_PLY) {
        return static_eval;
    }

    // Reverse futility pruning
    if constexpr (!is_pv_node(node_type)) {
        if (!old_board.in_check() && depth < 7 && (static_eval - (70 * depth)) >= beta) {
            return static_eval;
        }
    }

    if constexpr (!is_pv_node(node_type)) {
        if (!old_board.in_check() && static_eval < alpha - 400 - 250 * depth * depth) {
            const auto razoring_score = quiescent_search<NodeTypes::NON_PV_NODE>(old_board, alpha - 1, alpha, ply + 1, node_count);
            if (razoring_score < alpha) {
                return razoring_score;
            }
        }
    }

    if constexpr (!is_pv_node(node_type)) {
        if (static_eval >= beta && !old_board.in_check()) {
            // Try null move pruning if we aren't in check
            const auto& older_board = history[history.len() - 2];
            const bool nmp_stopped =
                old_board.pawns(Side::WHITE) == older_board.pawns(Side::WHITE) && old_board.pawns(Side::BLACK) == older_board.pawns(Side::BLACK)
                && old_board.knights(Side::WHITE) == older_board.knights(Side::WHITE)
                && old_board.knights(Side::BLACK) == older_board.knights(Side::BLACK)
                && old_board.bishops(Side::WHITE) == older_board.bishops(Side::WHITE)
                && old_board.bishops(Side::BLACK) == older_board.bishops(Side::BLACK)
                && old_board.rooks(Side::WHITE) == older_board.rooks(Side::WHITE) && old_board.rooks(Side::BLACK) == older_board.rooks(Side::BLACK)
                && old_board.queens(Side::WHITE) == older_board.queens(Side::WHITE)
                && old_board.queens(Side::BLACK) == older_board.queens(Side::BLACK) && old_board.kings(Side::WHITE) == older_board.kings(Side::WHITE)
                && old_board.kings(Side::BLACK) == older_board.kings(Side::BLACK);

            if (!nmp_stopped) {
                auto& board = old_board.make_move(Move::NULL_MOVE, history);
                
                const auto nmp_reduction = 4
                    + (depth / 4)
                    + std::min((static_eval - beta) / 200, 2);
                auto null_score =
                    -negamax_step<pv_node_type>(board, -beta, -alpha, depth - nmp_reduction, ply + 1, node_count, child_cutnode_type);

                history.pop_board();
                if (null_score >= beta) {
                    if (null_score > MagicNumbers::PositiveInfinity - MAX_PLY) {
                        return beta;
                    } else {
                        return null_score;
                    }
                }
            }
        }
    }

    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(old_board, old_board.get_side_to_move());
    if (moves.len() == 0) {
        if (old_board.in_check()) {
            // if in check
            return ply + MagicNumbers::NegativeInfinity;
        } else {
            return 0;
        }
    } else if (moves.len() == 1) {
        extensions += 1;
    }
    // mate and draw detection

    bool found_pv_move = false;
    MoveOrdering::reorder_moves(moves, old_board, tt_entry.key() == old_board.get_zobrist_key() ? tt_entry.move() : Move::NULL_MOVE, history_table,
                                search_stack[ply].killer_move, found_pv_move);
    const bool tt_move = found_pv_move && tt_hit;
    // move reordering

    if (depth >= 5 && !found_pv_move) {
        extensions -= 1;
    }
    // iir

    Move best_move = Move::NULL_MOVE;
    Score best_score = MagicNumbers::NegativeInfinity;
    const Score original_alpha = alpha;
    for (size_t evaluated_moves = 0; evaluated_moves < moves.len(); evaluated_moves++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[evaluated_moves];

        if constexpr (!is_pv_node(node_type)) {
            if (depth <= 6 && !old_board.in_check() && move.move.is_quiet() && evaluated_moves >= static_cast<size_t>((depth * depth) + 3)) {
                continue;
            }
        }

        if (!old_board.in_check() && best_score > (MagicNumbers::NegativeInfinity + MAX_PLY) && !move.move.is_capture() && depth <= 6
            && static_eval + 200 * depth < alpha) {
            continue;
        }

        if (depth <= 10 && best_score > (MagicNumbers::NegativeInfinity + MAX_PLY)
            && !Search::static_exchange_evaluation(old_board, move.move, move.move.is_capture() ? (-20 * depth * depth) : (-65 * depth))) {
            continue;
        }

        tt.prefetch(old_board.key_after(move.move));
        const auto pre_move_node_count = node_count;
        auto& board = old_board.make_move(move.move, history);
        node_count += 1;
        Score score;
        const auto new_depth = depth - 1 + extensions;

        // See if we can perform LMR
        if (depth > 2
            && evaluated_moves >= std::max((size_t) 1, static_cast<size_t>(is_pv_node(node_type)) + static_cast<size_t>(!tt_move)
                                                           + static_cast<size_t>(node_type == NodeTypes::ROOT_NODE)
                                                           + static_cast<size_t>(move.move.is_capture() || move.move.is_promotion()))) {
            const auto lmr_reduction =
                static_cast<int>(LmrTable[depth][evaluated_moves])
                + static_cast<int>(!is_pv_node(node_type) && is_cut_node)
                - static_cast<int>(board.in_check()); // Reduce less if the board is in check
            score = -negamax_step<NodeTypes::NON_PV_NODE>(board, -(alpha + 1), -alpha, new_depth - lmr_reduction, ply + 1, node_count,
                                                          child_cutnode_type);

            // it's possible the LMR score will raise alpha; in this case we re-search with the full depth
            if (score > alpha) {
                score = -negamax_step<NodeTypes::NON_PV_NODE>(board, -(alpha + 1), -alpha, new_depth, ply + 1, node_count,
                                                              child_cutnode_type);
            }
        }
        // if we didn't perform LMR
        else if (!is_pv_node(node_type) || evaluated_moves >= 1) {
            score =
                -negamax_step<NodeTypes::NON_PV_NODE>(board, -(alpha + 1), -alpha, new_depth, ply + 1, node_count, child_cutnode_type);
        }

        if (is_pv_node(node_type) && (evaluated_moves == 0 || score > alpha)) {
            score = -negamax_step<NodeTypes::PV_NODE>(board, -beta, -alpha, new_depth, ply + 1, node_count, child_cutnode_type);
        }

        history.pop_board();
        if constexpr (node_type == NodeTypes::ROOT_NODE) {
            node_spent_table[move.move.value() & 0x0FFF] += (node_count - pre_move_node_count);
        }
        if (score > best_score) {
            best_score = score;
            if (score > alpha) {
                best_move = move.move;
                if constexpr (node_type == NodeTypes::ROOT_NODE) {
                    pv_move = best_move;
                }
                if constexpr (is_pv_node(node_type)) {
                    pv_table.pv_array[ply][ply] = best_move;
                    for (int next_ply = ply + 1; next_ply < pv_table.pv_length[ply + 1]; next_ply++) {
                        pv_table.pv_array[ply][next_ply] = pv_table.pv_array[ply + 1][next_ply];
                    }
                    pv_table.pv_length[ply] = pv_table.pv_length[ply + 1];
                }
                if (score >= beta) {
                    search_stack[ply].killer_move = move.move;
                    for (size_t j = 0; j < evaluated_moves; j++) {
                        if (moves[j].move.is_quiet()) {
                            history_table[moves[j].move.get_history_idx(old_board.get_side_to_move())] -= (depth * depth);
                        }
                    }
                    if (!move.move.is_capture()) {
                        history_table[move.move.get_history_idx(old_board.get_side_to_move())] += (depth * depth);
                    }
                    break;
                }
                alpha = score;
            }
        }
    }
    const BoundTypes bound_type =
        (best_score >= beta ? BoundTypes::LOWER_BOUND : (alpha != original_alpha ? BoundTypes::EXACT_BOUND : BoundTypes::UPPER_BOUND));
    tt.store(TranspositionTableEntry(best_move, depth, bound_type, best_score, old_board.get_zobrist_key()), old_board);
    return best_score;
}

template <NodeTypes node_type>
Score SearchHandler::quiescent_search(const ChessBoard& old_board, Score alpha, Score beta, int ply, uint64_t& node_count) {
    if (Search::is_draw(old_board, history)) {
        return 0;
    }
    Score static_eval = Evaluation::evaluate_board(old_board);
    if (ply >= MAX_PLY) {
        return static_eval;
    }
    if (static_eval >= beta) {
        return static_eval;
    }
    alpha = std::max(static_eval, alpha);
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::QUIESCENCE>(old_board, old_board.get_side_to_move());
    if (moves.len() == 0 && MoveGenerator::generate_legal_moves<MoveGenType::NON_QUIESCENCE>(old_board, old_board.get_side_to_move()).len() == 0) {
        if (old_board.in_check()) {
            // if in check
            return ply + MagicNumbers::NegativeInfinity;
        } else {
            return 0;
        }
    }

    bool found_pv_move = false;
    Score best_score = static_eval;
    MoveOrdering::reorder_moves(moves, old_board, Move::NULL_MOVE, history_table, search_stack[ply].killer_move, found_pv_move);
    int evaluated_moves = 0;
    for (size_t i = 0; i < moves.len(); i++) {
        if (search_cancelled) {
            break;
        }
        const auto& move = moves[i];

        if (move.move.is_capture() && !move.move.is_promotion()) {
            if (!move.see_ordering_result) {
                continue;
            }
        } else {
            if (!Search::static_exchange_evaluation(old_board, move.move, -65)) {
                continue;
            }
        }

        auto& board = old_board.make_move(move.move, history);
        node_count += 1;
        Score score;
        if constexpr (is_pv_node(node_type)) {
            if (evaluated_moves == 0) {
                score = -quiescent_search<NodeTypes::PV_NODE>(board, -beta, -alpha, ply + 1, node_count);
            } else {
                score = -quiescent_search<NodeTypes::NON_PV_NODE>(board, -alpha - 1, -alpha, ply + 1, node_count);
                if (score > alpha) {
                    score = -quiescent_search<NodeTypes::PV_NODE>(board, -beta, -alpha, ply + 1, node_count);
                }
            }
        } else {
            score = -quiescent_search<NodeTypes::NON_PV_NODE>(board, -alpha - 1, -alpha, ply + 1, node_count);
        }

        history.pop_board();
        evaluated_moves += 1;
        if (score > best_score) {
            best_score = score;
            if (score >= beta) {
                search_stack[ply].killer_move = move.move;
                break;
            }
            alpha = std::max(score, alpha);
        }
    }
    return best_score;
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

        previous_score = negamax_step<NodeTypes::ROOT_NODE>(history[history.len() - 1], alpha, beta, depth, 0, node_count, false);

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
    auto moves =
        MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(history[history.len() - 1], history[history.len() - 1].get_side_to_move());
    // We generate legal moves only as it saves us having to continually rerun legality checks
    if (moves.len() == 1) {
        return moves[0].move;
        // If only one move is legal in this position we don't need to search; we can just return the one legal move
        // in order to save some time
    }

    history_table.fill(0);
    node_spent_table.fill(0);
    pv_table.pv_length.fill(0);
    for (unsigned int i = 0; i < pv_table.pv_array.size(); i++) {
        pv_table.pv_array[i].fill(Move::NULL_MOVE);
    }

    Score current_score = 0;
    for (int depth = 1; depth <= TimeManagement::get_search_depth(tc) && !search_cancelled; depth++) {

        current_score = run_aspiration_window_search(depth, current_score);
        const auto time_so_far = std::max(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - search_start_point).count(), (int64_t) 1);
        // Set time so far to a minimum of 1 to avoid divide by 0 in nps calculation

        if (!search_cancelled && print_info) {
            const auto nps = static_cast<uint64_t>(node_count / (static_cast<float>(time_so_far) / 1000));
            std::cout << "info depth " << depth << " nodes " << node_count << " nps " << nps << " score "
                      << ((std::abs(current_score) >= (MagicNumbers::PositiveInfinity - MAX_PLY))
                              ? ("mate " + std::to_string(((current_score / std::abs(current_score)) * (depth + 1)) / 2))
                              : ("cp " + std::to_string(current_score)))
                      << " time " << time_so_far << " pv ";
            for (int i = 0; i < pv_table.pv_length[0]; i++) {
                std::cout << pv_table.pv_array[0][i].to_string() << " ";
            }
            std::cout << std::endl;
        }

        if (current_score >= (MagicNumbers::PositiveInfinity - MAX_PLY)) {
            return pv_move;
        }

        if (TimeManagement::is_time_based_tc(tc) && time_so_far > TimeManagement::calculate_soft_limit(tc, node_spent_table, pv_move, node_count)) {
            break;
        }
    }
    return pv_move;
}