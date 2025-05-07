#include "move_generator.hpp"

#include <algorithm>
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

bool MoveGenerator::can_castle(const Position& pos, const Side side, const bool is_kingside) {
    const auto enemy = enemy_side(side);
    const auto king_sq = pos.kings(side).lsb();
    const auto side_rnk = side == Side::WHITE ? 0 : 7;

    const auto king_dest_fle = is_kingside ? 6 : 2;
    const auto rook_dest_fle = is_kingside ? 5 : 3;
    const auto rook_dest_sq = square(side_rnk, rook_dest_fle);

    const auto rook_file = pos.castling_file(side, is_kingside);
    const auto rook_sq = square(side_rnk, rook_file);
    const auto occupancy_bb = pos.occupancy() ^ king_sq ^ rook_sq;
    const auto king_dest_sq = square(side_rnk, king_dest_fle);
    const auto king_min_sq = std::min(king_sq, king_dest_sq);
    const auto king_max_sq = std::max(king_sq, king_dest_sq);
    auto _king_movement_bb = Bitboard(0);
    for (u8 i = sq_to_int(king_min_sq); i <= sq_to_int(king_max_sq); i++) {
        _king_movement_bb |= static_cast<Square>(i);
    }
    auto _movement_bb = _king_movement_bb;
    const auto rook_min_sq = std::min(rook_sq, rook_dest_sq);
    const auto rook_max_sq = std::max(rook_sq, rook_dest_sq);
    for (u8 i = sq_to_int(rook_min_sq); i <= sq_to_int(rook_max_sq); i++) {
        _movement_bb |= static_cast<Square>(i);
    }
    if (!(occupancy_bb & _movement_bb).empty()) {
        return false;
    }
    // No pieces obstruct either king or rook movement
    if (_king_movement_bb.popcnt() > 1) {
        _king_movement_bb &= ~Bitboard(king_sq); // Clear the origin square as this has already been evaluated
        // if a king castles to itself we need to check this again
    }
    const auto blocker_bb = occupancy_bb | rook_dest_sq;
    while (!_king_movement_bb.empty()) {
        // Iterate over every remaining square to check for check
        const auto sq = _king_movement_bb.pop_lsb();
        if (!get_attackers(pos, enemy, sq, blocker_bb).empty()) {
            return false; // break early
        }
    }
    return true;
}

void MoveGenerator::generate_castling_moves(const Position& c, const Side side, MoveList& move_list) {
    const auto king_sq = c.kings(side).lsb();
    if (c.get_kingside_castling(side) && can_castle(c, side, true)) {
        const auto rook_file = c.castling_file(side, true);
        const auto rook_sq = square(side == Side::WHITE ? 0 : 7, rook_file);
        move_list.add(Move(MoveFlags::KINGSIDE_CASTLE, rook_sq, king_sq));
    }
    if (c.get_queenside_castling(side) && can_castle(c, side, false)) {
        const auto rook_file = c.castling_file(side, false);
        const auto rook_sq = square(side == Side::WHITE ? 0 : 7, rook_file);
        move_list.add(Move(MoveFlags::QUEENSIDE_CASTLE, rook_sq, king_sq));
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
    if (m.flags() == MoveFlags::EN_PASSANT_CAPTURE) {
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
    } else if (m.is_castling_move()) {
        if (c.in_check()) {
            return false; // We can't castle when in check
        }
        return can_castle(c, c.stm(), m.flags() == MoveFlags::KINGSIDE_CASTLE);
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
            if (pos.in_check()) return false;
            generate_castling_moves(pos, stm, generated_moves);
            return std::find_if(generated_moves.begin(), generated_moves.end(), [&](ScoredMove s){ return s.move == m; }) != generated_moves.end();
        } else if (m.is_promotion()) {
            generate_pawn_moves<MoveGenType::ALL_LEGAL>(pos, stm, generated_moves);
            return std::find_if(generated_moves.begin(), generated_moves.end(), [&](ScoredMove s){ return s.move == m; }) != generated_moves.end();
        } else if (m.flags() == MoveFlags::EN_PASSANT_CAPTURE) {
            generate_pawn_moves<MoveGenType::NOISY>(pos, stm, generated_moves);
            return std::find_if(generated_moves.begin(), generated_moves.end(), [&](ScoredMove s){ return s.move == m; }) != generated_moves.end();
        }
    }

    // We must move a friendly piece
    if (moved_pc == 0 || moved_pc.side() != stm) {
        return false;
    }

    // Make sure a friendly piece is not at the destination
    if (!(pos.occupancy(stm) & to).empty()) {
        return false;
    }

    // Ensure that captures are captures
    if (m.is_capture() && (pos.occupancy(enemy_side(stm)) & to).empty()) {
        return false;
    }

    // If an enemy piece exists and we're doing a quiet move
    if (m.flags() == MoveFlags::QUIET_MOVE && !(pos.occupancy(enemy_side(stm)) & to).empty()) {
        return false;
    }

    if (moved_pc.type() == PieceTypes::PAWN) {
        // Ensure this isn't a promotion
        if (!((rank_bb(0) | rank_bb(7)) & to).empty()) {
            return false;
        }

        const auto can_capture = !(MagicNumbers::PawnAttacks[static_cast<int>(stm)][sq_to_int(from)] & pos.occupancy(enemy_side(stm)) & to).empty();
        if (m.is_capture() && !can_capture) return false;
        const auto to_empty = (pos.occupancy() & to).empty();
        const auto pawn_push = stm == Side::WHITE ? 8 : -8;
        const auto can_single_push = (static_cast<Square>(sq_to_int(from) + pawn_push) == to) && to_empty;
        if (m.flags() == MoveFlags::QUIET_MOVE && !can_single_push) return false;
        const auto start_rank = stm == Side::WHITE ? 1 : 6;
        const auto can_double_push = (static_cast<Square>(sq_to_int(from) + (2 * pawn_push)) == to)
                                        && m.src_rnk() == start_rank
                                        && to_empty
                                        && (pos.occupancy() & static_cast<Square>(sq_to_int(to) - pawn_push)).empty();
        if (m.flags() == MoveFlags::DOUBLE_PAWN_PUSH && !can_double_push) return false;
    } else if ((generate_mm(moved_pc.type(), pos.occupancy(), from) & to).empty() || m.flags() == MoveFlags::DOUBLE_PAWN_PUSH) {
        return false;
    }

    if (pos.in_check()) {
        if (moved_pc.type() != PieceTypes::KING) {
            // only king moves are legal in double check
            if (pos.checkers().popcnt() > 1) {
                return false;
            }

            if (MagicNumbers::ConnectingSquares[sq_to_int(pos.kings(stm).lsb())][sq_to_int(pos.checkers().lsb())].empty()) {
                return false;
            }
        } else if (!get_attackers(pos, enemy_side(stm), to, pos.occupancy() ^ from).empty()) {
            return false;
        }
    }
    return true;
}
