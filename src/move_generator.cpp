#include "move_generator.hpp"

#include <bit>

#include "magic_numbers.hpp"

int MoveGenerator::get_checking_piece_count(const Position& c, const Side side) { return MoveGenerator::get_checkers(c, side).popcnt(); }

Bitboard MoveGenerator::get_checkers(const Position& c, const Side side) {
    const Side enemy = enemy_side(side);

    return get_attackers(c, enemy, c.kings(side).lsb(), c.occupancy());
}

/**
 * @brief Gets the pieces on side side that attack the piece at position target_idx
 *
 * @param board
 * @param side
 * @param target_idx
 * @param occupancy
 * @return Bitboard
 */
Bitboard MoveGenerator::get_attackers(const Position& board, const Side side, const Square target_sq, const Bitboard occupancy) {
    const Side enemy = enemy_side(side);
    Bitboard bishop_mask = MoveGenerator::generate_bishop_mm(occupancy, target_sq);
    Bitboard rook_mask = MoveGenerator::generate_rook_mm(occupancy, target_sq);

    Bitboard to_return = 0;

    to_return |= board.queens(side) & (bishop_mask | rook_mask);
    to_return |= board.bishops(side) & bishop_mask;
    to_return |= board.rooks(side) & rook_mask;
    to_return |= board.knights(side) & MagicNumbers::KnightMoves[sq_to_int(target_sq)];
    to_return |= board.pawns(side) & MagicNumbers::PawnAttacks[static_cast<int>(enemy)][sq_to_int(target_sq)];
    to_return |= board.kings(side) & MagicNumbers::KingMoves[sq_to_int(target_sq)];

    return to_return;
}

Bitboard MoveGenerator::generate_bishop_mm(const Bitboard b, const Square sq) {
    const auto idx = sq_to_int(sq);
    Bitboard masked = (b & MagicNumbers::BishopMasks[idx]);
    return MagicNumbers::BishopAttacks[idx][((masked * MagicNumbers::BishopMagics[idx]) >> (64 - MagicNumbers::BishopBits[idx]))];
}

Bitboard MoveGenerator::generate_rook_mm(const Bitboard b, const Square sq) {
    const auto idx = sq_to_int(sq);
    Bitboard masked = (b & MagicNumbers::RookMasks[idx]);
    return MagicNumbers::RookAttacks[idx][((masked * MagicNumbers::RookMagics[idx]) >> (64 - MagicNumbers::RookBits[idx]))];
}

Bitboard MoveGenerator::generate_queen_mm(const Bitboard b, const Square sq) {
    return MoveGenerator::generate_bishop_mm(b, sq) | MoveGenerator::generate_rook_mm(b, sq);
}

Bitboard MoveGenerator::generate_mm(const PieceTypes pc_type, const Bitboard occupancy, const Square sq) {
    switch (pc_type) {
        case PieceTypes::KNIGHT:
            return generate_mm<PieceTypes::KNIGHT>(occupancy, sq);
        case PieceTypes::BISHOP:
            return generate_mm<PieceTypes::BISHOP>(occupancy, sq);
        case PieceTypes::ROOK:
            return generate_mm<PieceTypes::ROOK>(occupancy, sq);
        case PieceTypes::QUEEN:
            return generate_mm<PieceTypes::QUEEN>(occupancy, sq);
        case PieceTypes::KING:
            return generate_mm<PieceTypes::KING>(occupancy, sq);
        default:
            return 0;
    }
}

void MoveGenerator::generate_castling_moves(const Position& c, const Side side, MoveList& move_list) {
    const Bitboard total_occupancy = c.occupancy();
    const auto enemy = enemy_side(side);
    if (c.get_kingside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if ((Bitboard(0b10010000) ^ ((c.occupancy() >> shift_val) & Bitboard(0xF0))).empty()) {
            // if only these spaces are occupied
            if (get_attackers(c, enemy, static_cast<Square>(5 + shift_val), total_occupancy).empty() && get_attackers(c, enemy, static_cast<Square>(6 + shift_val), total_occupancy).empty()) {
                move_list.add(Move(MoveFlags::KINGSIDE_CASTLE, 6 + shift_val, 4 + shift_val));
            }
        }
    }
    if (c.get_queenside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if ((Bitboard(0b00010001) ^ ((c.occupancy() >> shift_val) & Bitboard(0x1F))).empty()) {
            // if only these spaces are occupied
            if (get_attackers(c, enemy, static_cast<Square>(3 + shift_val), total_occupancy).empty() && get_attackers(c, enemy, static_cast<Square>(2 + shift_val), total_occupancy).empty()) {
                move_list.add(Move(MoveFlags::QUEENSIDE_CASTLE, 2 + shift_val, 4 + shift_val));
            }
        }
    }
}

/**
 * @brief Checks if a pseudolegal move on a position is legal; algorithm stolen from Stockfish
 * 
 * @param c 
 * @param m 
 * @return true 
 * @return false 
 */
bool MoveGenerator::is_move_legal(const Position& c, const Move m) {
    const auto king_idx = c.kings(c.stm()).lsb();
    const auto move_side = static_cast<Side>(c.occupancy(Side::BLACK)[m.src_sq()]);
    const Side enemy = enemy_side(move_side);
    if (m.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        // with en passant knights and pawns _cannot_ capture as the previous
        // move was moving a pawn, and therefore knights/pawns were not in
        // position to capture-they'd have needed to be moved two moves ago,
        // and we would have moved out of check
        Bitboard occupancy = c.occupancy();
        Bitboard cleared_occupancy =
            occupancy ^ (Bitboard(m.src_sq()) | Bitboard(m.dst_sq() - 8 + (16 * static_cast<int>(c.stm()))));
        // clear the origin and capture spaces
        // then set the destination square
        cleared_occupancy |= m.dst_sq();
        return !((MoveGenerator::generate_bishop_mm(cleared_occupancy, king_idx) & (c.bishops(enemy) | c.queens(enemy)))
                 || (MoveGenerator::generate_rook_mm(cleared_occupancy, king_idx) & (c.rooks(enemy) | c.queens(enemy))));
    } else if (c.kings()[m.src_sq()]) {
        Bitboard cleared_bitboard = c.occupancy() ^ m.src_sq();
        const auto target_idx = m.dst_sq();
        const auto potential_diagonal_sliders = (c.bishops(enemy) | c.queens(enemy));
        const auto potential_orthogonal_sliders = (c.rooks(enemy) | c.queens(enemy));
        return !((generate_bishop_mm(cleared_bitboard, target_idx) & potential_diagonal_sliders)
                 || (generate_rook_mm(cleared_bitboard, target_idx) & potential_orthogonal_sliders)
                 || (c.knights(enemy) & MagicNumbers::KnightMoves[sq_to_int(target_idx)])
                 || (c.pawns(enemy) & MagicNumbers::PawnAttacks[static_cast<int>(move_side)][sq_to_int(target_idx)])
                 || (c.kings(enemy) & MagicNumbers::KingMoves[sq_to_int(target_idx)]));
    } else [[likely]] {

        Bitboard checking_pieces = c.checkers();
        if (!checking_pieces.empty()) {
            // if there's a piece checking our king - we know at most one piece can be checking as double checks are king moves only
            if (!(MagicNumbers::ConnectingSquares[sq_to_int(king_idx)][sq_to_int(checking_pieces.lsb())] & m.dst_sq())) {
                return false;
            }
        }

        if (!(c.pinned_pieces() & m.src_sq()).empty()) {
            return !(MagicNumbers::AlignedSquares[sq_to_int(king_idx)][sq_to_int(m.src_sq())] & m.dst_sq()).empty();
        }
    }

    return true;
}

/**
 * @brief Checks if a move on a position is pseudolegal; algorithm stolen from Stockfish
 * 
 * @param pos 
 * @param m 
 * @return true 
 * @return false 
 */
bool MoveGenerator::is_move_pseudolegal(const Position& pos, const Move m) {
    const auto stm = pos.stm();
    const auto from = m.src_sq();
    const auto to = m.dst_sq();
    const auto moved_pc = pos.piece_at(from);

    {
        MoveList generated_moves;
        if (m.is_castling_move()) {
            generate_castling_moves(pos, stm, generated_moves);
            return std::find_if(generated_moves.begin(), generated_moves.end(), [&](ScoredMove s){ return s.move == m; }) != generated_moves.end();
        } else if (m.is_promotion()) {
            if (stm == Side::WHITE) {
                generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, generated_moves);
            } else {
                generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, generated_moves);
            }
            return std::find_if(generated_moves.begin(), generated_moves.end(), [&](ScoredMove s){ return s.move == m; }) != generated_moves.end();
        } else if (m.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
            if (stm == Side::WHITE) {
                generate_pawn_moves<MoveGenType::CAPTURES, Side::WHITE>(pos, generated_moves);
            } else {
                generate_pawn_moves<MoveGenType::CAPTURES, Side::BLACK>(pos, generated_moves);
            }
            return std::find_if(generated_moves.begin(), generated_moves.end(), [&](ScoredMove s){ return s.move == m; }) != generated_moves.end();
        }
    }

    // We must move a friendly piece
    if (moved_pc == 0 || moved_pc.get_side() != stm) {
        return false;
    }

    // Make sure a friendly piece is not at the destination
    if (!(pos.occupancy(stm) & to).empty()) {
        return false;
    }

    if (moved_pc.get_type() == PieceTypes::PAWN) {
        // Ensure this isn't a promotion
        if (!((rank_bb(0) | rank_bb(7)) & to).empty()) {
            return false;
        }

        const auto can_capture = !(MagicNumbers::PawnAttacks[static_cast<int>(stm)][sq_to_int(from)] & pos.occupancy(enemy_side(stm)) & to).empty();
        if (m.is_capture() && !can_capture) return false;
        const auto to_empty = (pos.occupancy() & to).empty();
        const auto pawn_push = stm == Side::WHITE ? 8 : -8;
        const auto can_single_push = (static_cast<Square>(sq_to_int(from) + pawn_push) == to) && to_empty;
        if (m.get_move_flags() == MoveFlags::QUIET_MOVE && !can_single_push) return false;
        const auto start_rank = stm == Side::WHITE ? 1 : 6;
        const auto can_double_push = (static_cast<Square>(sq_to_int(from) + (2 * pawn_push)) == to)
                                        && m.src_rnk() == start_rank
                                        && to_empty
                                        && (pos.occupancy() & static_cast<Square>(sq_to_int(to) - pawn_push)).empty();
        if (m.get_move_flags() == MoveFlags::DOUBLE_PAWN_PUSH && !can_double_push) return false;
    } else if ((generate_mm(moved_pc.get_type(), pos.occupancy(), from) & to).empty()) {
        return false;
    }

    if (pos.in_check()) {
        if (moved_pc.get_type() != PieceTypes::KING) {
            // only king moves are legal in double check
            if (pos.checkers().popcnt() > 1) {
                return false;
            }

            if (MagicNumbers::ConnectingSquares[(64 * sq_to_int(pos.kings(stm).lsb())) + sq_to_int(pos.checkers().lsb())].empty()) {
                return false;
            }
        } else if (!get_attackers(pos, enemy_side(stm), to, pos.occupancy() ^ from).empty()) {
            return false;
        }
    }
    return true;
}
