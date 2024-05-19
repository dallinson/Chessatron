#include "move_generator.hpp"

#include <bit>

#include "magic_numbers.hpp"

int MoveGenerator::get_checking_piece_count(const ChessBoard& c, const Side side) { return std::popcount(MoveGenerator::get_checkers(c, side)); }

Bitboard MoveGenerator::get_checkers(const ChessBoard& c, const Side side) {
    const Side enemy = enemy_side(side);

    return get_attackers(c, enemy, get_lsb(c.kings(side)), c.occupancy());
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
Bitboard MoveGenerator::get_attackers(const ChessBoard& board, const Side side, const int target_idx, const Bitboard occupancy) {
    const Side enemy = enemy_side(side);
    Bitboard bishop_mask = MoveGenerator::generate_bishop_mm(occupancy, target_idx);
    Bitboard rook_mask = MoveGenerator::generate_rook_mm(occupancy, target_idx);

    Bitboard to_return = 0;

    to_return |= board.queens(side) & (bishop_mask | rook_mask);
    to_return |= board.bishops(side) & bishop_mask;
    to_return |= board.rooks(side) & rook_mask;
    to_return |= board.knights(side) & MagicNumbers::KnightMoves[target_idx];
    to_return |= board.pawns(side) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(enemy)) + target_idx];
    to_return |= board.kings(side) & MagicNumbers::KingMoves[target_idx];

    return to_return;
}

Bitboard MoveGenerator::generate_bishop_mm(const Bitboard b, const int idx) {
    Bitboard masked = (b & MagicNumbers::BishopMasks[idx]);
    return MagicNumbers::BishopAttacks[(512 * idx) + ((masked * MagicNumbers::BishopMagics[idx]) >> (64 - MagicNumbers::BishopBits[idx]))];
}

Bitboard MoveGenerator::generate_rook_mm(const Bitboard b, const int idx) {
    Bitboard masked = (b & MagicNumbers::RookMasks[idx]);
    return MagicNumbers::RookAttacks[(4096 * idx) + ((masked * MagicNumbers::RookMagics[idx]) >> (64 - MagicNumbers::RookBits[idx]))];
}

Bitboard MoveGenerator::generate_queen_mm(const Bitboard b, const int idx) {
    return MoveGenerator::generate_bishop_mm(b, idx) | MoveGenerator::generate_rook_mm(b, idx);
}

void MoveGenerator::generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    const Bitboard total_occupancy = c.occupancy();
    const auto enemy = enemy_side(side);
    if (c.get_kingside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b10010000 ^ ((c.occupancy() >> shift_val) & 0xF0)) == 0) {
            // if only these spaces are occupied
            if (get_attackers(c, enemy, 5 + shift_val, total_occupancy) == 0 && get_attackers(c, enemy, 6 + shift_val, total_occupancy) == 0) {
                move_list.add_move(Move(MoveFlags::KINGSIDE_CASTLE, 6 + shift_val, 4 + shift_val));
            }
        }
    }
    if (c.get_queenside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b00010001 ^ ((c.occupancy() >> shift_val) & 0x1F)) == 0) {
            // if only these spaces are occupied
            if (get_attackers(c, enemy, 3 + shift_val, total_occupancy) == 0 && get_attackers(c, enemy, 2 + shift_val, total_occupancy) == 0) {
                move_list.add_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2 + shift_val, 4 + shift_val));
            }
        }
    }
}

bool MoveGenerator::is_move_legal(const ChessBoard& c, const Move m) {
    int king_idx = get_lsb(c.kings(c.get_side_to_move()));
    const auto move_side = static_cast<Side>(get_bit(c.occupancy(Side::BLACK), m.src_sq()));
    const Side enemy = enemy_side(move_side);
    if (m.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        // with en passant knights and pawns _cannot_ capture as the previous
        // move was moving a pawn, and therefore knights/pawns were not in
        // position to capture-they'd have needed to be moved two moves ago,
        // and we would have moved out of check
        Bitboard occupancy = c.occupancy();
        Bitboard cleared_occupancy =
            occupancy ^ (idx_to_bb(m.src_sq()) | idx_to_bb(m.dst_sq() - 8 + (16 * static_cast<int>(c.get_side_to_move()))));
        // clear the origin and capture spaces
        // then set the destination square
        cleared_occupancy |= idx_to_bb(m.dst_sq());
        return !((MoveGenerator::generate_bishop_mm(cleared_occupancy, king_idx) & (c.bishops(enemy) | c.queens(enemy)))
                 || (MoveGenerator::generate_rook_mm(cleared_occupancy, king_idx) & (c.rooks(enemy) | c.queens(enemy))));
    } else if (get_bit(c.kings(), m.src_sq()) != 0) {
        Bitboard cleared_bitboard = c.occupancy() ^ idx_to_bb(m.src_sq());
        int target_idx = m.dst_sq();
        const auto potential_diagonal_sliders = (c.bishops(enemy) | c.queens(enemy));
        const auto potential_orthogonal_sliders = (c.rooks(enemy) | c.queens(enemy));
        return !((generate_bishop_mm(cleared_bitboard, target_idx) & potential_diagonal_sliders)
                 || (generate_rook_mm(cleared_bitboard, target_idx) & potential_orthogonal_sliders)
                 || (c.knights(enemy) & MagicNumbers::KnightMoves[target_idx])
                 || (c.pawns(enemy) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(move_side)) + target_idx])
                 || (c.kings(enemy) & MagicNumbers::KingMoves[target_idx]));
    } else [[likely]] {

        Bitboard checking_pieces = c.checkers();
        if (checking_pieces) {
            // if there's a piece checking our king - we know at most one piece can be checking as double checks are king moves only
            if (!(MagicNumbers::ConnectingSquares[(64 * king_idx) + get_lsb(checking_pieces)] & idx_to_bb(m.dst_sq()))) {
                return false;
            }
        }

        if (idx_to_bb(m.src_sq()) & c.pinned_pieces()) {
            return MagicNumbers::AlignedSquares[(64 * king_idx) + m.src_sq()] & idx_to_bb(m.dst_sq());
        }
    }

    return true;
}

bool MoveGenerator::is_move_pseudolegal(const ChessBoard& board, const Move move) {
    MoveList to_generate;
    const auto moved_pc = board.piece_at(move.src_sq());
    const auto stm = board.get_side_to_move();
    if (moved_pc.get_type() == PieceTypes::PAWN) {
        if (stm == Side::WHITE) {
            generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(board, to_generate);
        } else {
            generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(board, to_generate);
        }
    } else if (moved_pc.get_type() == PieceTypes::KNIGHT) {
        generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(board, stm, to_generate);
    } else if (moved_pc.get_type() == PieceTypes::BISHOP) {
        generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(board, stm, to_generate);
    } else if (moved_pc.get_type() == PieceTypes::ROOK) {
        generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(board, stm, to_generate);
    } else if (moved_pc.get_type() == PieceTypes::QUEEN) {
        generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(board, stm, to_generate);
    } else if (moved_pc.get_type() == PieceTypes::KING) {
        generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(board, stm, to_generate);
    }
    return std::find_if(to_generate.begin(), to_generate.end(), [&](ScoredMove s) { return s.move == move; }) != to_generate.end();
}