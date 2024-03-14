#include "move_generator.hpp"

#include <bit>

#include "magic_numbers.hpp"

int MoveGenerator::get_checking_piece_count(const ChessBoard& c, const Side side) {
    return std::popcount(MoveGenerator::get_checkers(c, side));
}

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
    const auto move_side = static_cast<Side>(get_bit(c.occupancy(Side::BLACK), m.get_src_square()));
    const Side enemy = enemy_side(move_side);
    if (m.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        // with en passant knights and pawns _cannot_ capture as the previous
        // move was moving a pawn, and therefore knights/pawns were not in
        // position to capture-they'd have needed to be moved two moves ago,
        // and we would have moved out of check
        Bitboard occupancy = c.occupancy();
        Bitboard cleared_occupancy = occupancy ^ (idx_to_bb(m.get_src_square()) |
                                                  idx_to_bb(m.get_dest_square() - 8 + (16 * static_cast<int>(c.get_side_to_move()))));
        // clear the origin and capture spaces
        // then set the destination square
        cleared_occupancy |= idx_to_bb(m.get_dest_square());
        return !((MoveGenerator::generate_bishop_mm(cleared_occupancy, king_idx) &
                  (c.bishops(enemy) | c.queens(enemy))) ||
                 (MoveGenerator::generate_rook_mm(cleared_occupancy, king_idx) &
                  (c.rooks(enemy) | c.queens(enemy))));
    } else if (get_bit(c.kings(), m.get_src_square()) != 0) {
        Bitboard cleared_bitboard = c.occupancy() ^ idx_to_bb(m.get_src_square());
        int target_idx = m.get_dest_square();
        const auto potential_diagonal_sliders = (c.bishops(enemy) | c.queens(enemy));
        const auto potential_orthogonal_sliders = (c.rooks(enemy) | c.queens(enemy));
        return !((generate_bishop_mm(cleared_bitboard, target_idx) & potential_diagonal_sliders) ||
                 (generate_rook_mm(cleared_bitboard, target_idx) & potential_orthogonal_sliders) ||
                 (c.knights(enemy) & MagicNumbers::KnightMoves[target_idx]) ||
                 (c.pawns(enemy) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(move_side)) + target_idx]) ||
                 (c.kings(enemy) & MagicNumbers::KingMoves[target_idx]));
    } else [[likely]] {

        Bitboard checking_pieces = c.get_checkers();
        if (checking_pieces) {
            // if there's a piece checking our king - we know at most one piece can be checking as double checks are king moves only
            if (!(MagicNumbers::ConnectingSquares[(64 * king_idx) + get_lsb(checking_pieces)] & idx_to_bb(m.get_dest_square()))) {
                return false;
            }
        }

        if (idx_to_bb(m.get_src_square()) & c.get_pinned_pieces()) {
            return MagicNumbers::AlignedSquares[(64 * king_idx) + m.get_src_square()] & idx_to_bb(m.get_dest_square());
        }
    }

    return true;
}

bool MoveGenerator::is_move_pseudolegal(const ChessBoard& c, const Move to_test) {
    auto src_idx = to_test.get_src_square();
    auto moved_piece = c.piece_at(src_idx);
    if (moved_piece.get_value() == 0) {
        return false;
    }
    auto move_side = moved_piece.get_side();
    auto piece_type = moved_piece.get_type();
    if (move_side != c.get_side_to_move()) {
        return false;
    }
    MoveList moves;
    switch (piece_type) {
    case PieceTypes::PAWN:
        generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, move_side, moves);
        break;
    case PieceTypes::KNIGHT:
        generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(c, move_side, moves);
        break;
    case PieceTypes::BISHOP:
        generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(c, move_side, moves);
        break;
    case PieceTypes::ROOK:
        generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(c, move_side, moves);
        break;
    case PieceTypes::QUEEN:
        generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(c, move_side, moves);
        break;
    case PieceTypes::KING:
        generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(c, move_side, moves);
        break;
    }

    for (size_t i = 0; i < moves.len(); i++) {
        if (moves[i].move == to_test) {
            return true;
        }
    }
    return false;
}
