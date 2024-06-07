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
    to_return |= board.pawns(side) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(enemy)) + sq_to_int(target_sq)];
    to_return |= board.kings(side) & MagicNumbers::KingMoves[sq_to_int(target_sq)];

    return to_return;
}

Bitboard MoveGenerator::generate_bishop_mm(const Bitboard b, const Square sq) {
    const auto idx = sq_to_int(sq);
    Bitboard masked = (b & MagicNumbers::BishopMasks[idx]);
    return MagicNumbers::BishopAttacks[(512 * idx) + ((masked * MagicNumbers::BishopMagics[idx]) >> (64 - MagicNumbers::BishopBits[idx]))];
}

Bitboard MoveGenerator::generate_rook_mm(const Bitboard b, const Square sq) {
    const auto idx = sq_to_int(sq);
    Bitboard masked = (b & MagicNumbers::RookMasks[idx]);
    return MagicNumbers::RookAttacks[(4096 * idx) + ((masked * MagicNumbers::RookMagics[idx]) >> (64 - MagicNumbers::RookBits[idx]))];
}

Bitboard MoveGenerator::generate_queen_mm(const Bitboard b, const Square sq) {
    return MoveGenerator::generate_bishop_mm(b, sq) | MoveGenerator::generate_rook_mm(b, sq);
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
                 || (c.pawns(enemy) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(move_side)) + sq_to_int(target_idx)])
                 || (c.kings(enemy) & MagicNumbers::KingMoves[sq_to_int(target_idx)]));
    } else [[likely]] {

        Bitboard checking_pieces = c.checkers();
        if (!checking_pieces.empty()) {
            // if there's a piece checking our king - we know at most one piece can be checking as double checks are king moves only
            if (!(MagicNumbers::ConnectingSquares[(64 * sq_to_int(king_idx)) + sq_to_int(checking_pieces.lsb())] & m.dst_sq())) {
                return false;
            }
        }

        if (!(c.pinned_pieces() & m.src_sq()).empty()) {
            return !(MagicNumbers::AlignedSquares[(64 * sq_to_int(king_idx)) + sq_to_int(m.src_sq())] & m.dst_sq()).empty();
        }
    }

    return true;
}
