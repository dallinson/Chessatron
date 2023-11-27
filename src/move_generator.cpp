#include "move_generator.hpp"

#include <bit>

#include "magic_numbers.hpp"

int MoveGenerator::get_checking_piece_count(const ChessBoard& c, const Side side) {
    return std::popcount(MoveGenerator::get_checkers(c, side));
}

Bitboard MoveGenerator::get_checkers(const ChessBoard& c, const Side side) {
    const Side enemy_side = ENEMY_SIDE(side);

    return get_attackers(c, enemy_side, bitboard_to_idx(c.get_king_occupancy(side)), c.get_occupancy());
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
    const Side enemy_side = ENEMY_SIDE(side);
    Bitboard bishop_mask = MoveGenerator::generate_bishop_movemask(occupancy, target_idx);
    Bitboard rook_mask = MoveGenerator::generate_rook_movemask(occupancy, target_idx);

    Bitboard to_return = 0;

    to_return |= board.get_queen_occupancy(side) & (bishop_mask | rook_mask);
    to_return |= board.get_bishop_occupancy(side) & bishop_mask;
    to_return |= board.get_rook_occupancy(side) & rook_mask;
    to_return |= board.get_knight_occupancy(side) & MagicNumbers::KnightMoves[target_idx];
    to_return |= board.get_pawn_occupancy(side) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(enemy_side)) + target_idx];
    to_return |= board.get_king_occupancy(side) & MagicNumbers::KingMoves[target_idx];

    return to_return;
}

Bitboard MoveGenerator::generate_bishop_movemask(const Bitboard b, const int idx) {
    Bitboard masked = (b & MagicNumbers::BishopMasks[idx]);
    return MagicNumbers::BishopAttacks[(512 * idx) + ((masked * MagicNumbers::BishopMagics[idx]) >> (64 - MagicNumbers::BishopBits[idx]))];
}

Bitboard MoveGenerator::generate_rook_movemask(const Bitboard b, const int idx) {
    Bitboard masked = (b & MagicNumbers::RookMasks[idx]);
    return MagicNumbers::RookAttacks[(4096 * idx) + ((masked * MagicNumbers::RookMagics[idx]) >> (64 - MagicNumbers::RookBits[idx]))];
}

Bitboard MoveGenerator::generate_queen_movemask(const Bitboard b, const int idx) {
    return MoveGenerator::generate_bishop_movemask(b, idx) | MoveGenerator::generate_rook_movemask(b, idx);
}

Bitboard MoveGenerator::generate_movemask(const PieceTypes piece_type, const Bitboard b, const int idx) {
    switch (piece_type) {
    case PieceTypes::BISHOP:
        return generate_bishop_movemask(b, idx);
    case PieceTypes::KING:
        return MagicNumbers::KingMoves[idx];
    case PieceTypes::KNIGHT:
        return MagicNumbers::KnightMoves[idx];
    case PieceTypes::QUEEN:
        return generate_queen_movemask(b, idx);
    case PieceTypes::ROOK:
        return generate_rook_movemask(b, idx);
    case PieceTypes::PAWN:
        break;
    }
    return 0;
}

void MoveGenerator::generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    const Bitboard total_occupancy = c.get_occupancy();
    const auto enemy_side = ENEMY_SIDE(side);
    if (c.get_kingside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b10010000 ^ ((c.get_occupancy() >> shift_val) & 0xF0)) == 0) {
            // if only these spaces are occupied
            if (get_attackers(c, enemy_side, 5 + shift_val, total_occupancy) == 0 && get_attackers(c, enemy_side, 6 + shift_val, total_occupancy) == 0) {
                move_list.add_move(Move(MoveFlags::KINGSIDE_CASTLE, 6 + shift_val, 4 + shift_val));
            }
        }
    }
    if (c.get_queenside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b00010001 ^ ((c.get_occupancy() >> shift_val) & 0x1F)) == 0) {
            // if only these spaces are occupied
            if (get_attackers(c, enemy_side, 3 + shift_val, total_occupancy) == 0 && get_attackers(c, enemy_side, 2 + shift_val, total_occupancy) == 0) {
                move_list.add_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2 + shift_val, 4 + shift_val));
            }
        }
    }
}

bool MoveGenerator::is_move_legal(const ChessBoard& c, const Move m) {
    int king_idx = bitboard_to_idx(c.get_king_occupancy(c.get_side_to_move()));
    const auto move_side = c.get_piece(m.get_src_square()).get_side();
    const Side enemy_side = ENEMY_SIDE(move_side);
    if (m.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        // with en passant knights and pawns _cannot_ capture as the previous
        // move was moving a pawn, and therefore knights/pawns were not in
        // position to capture-they'd have needed to be moved two moves ago,
        // and we would have moved out of check
        Bitboard occupancy = c.get_occupancy();
        Bitboard cleared_occupancy = occupancy ^ (idx_to_bitboard(m.get_src_square()) |
                                                  idx_to_bitboard(m.get_dest_square() - 8 + (16 * static_cast<int>(c.get_side_to_move()))));
        // clear the origin and capture spaces
        // then set the destination square
        cleared_occupancy |= idx_to_bitboard(m.get_dest_square());
        return !((MoveGenerator::generate_bishop_movemask(cleared_occupancy, king_idx) &
                  (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
                 (MoveGenerator::generate_rook_movemask(cleared_occupancy, king_idx) &
                  (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))));
    } else if (c.get_piece(m.get_src_square()).get_type() == PieceTypes::KING) {
        Bitboard cleared_bitboard = c.get_occupancy() ^ idx_to_bitboard(m.get_src_square());
        int target_idx = m.get_dest_square();
        const auto potential_diagonal_sliders = (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));
        const auto potential_orthogonal_sliders = (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));
        return !((generate_bishop_movemask(cleared_bitboard, target_idx) & potential_diagonal_sliders) ||
                 (generate_rook_movemask(cleared_bitboard, target_idx) & potential_orthogonal_sliders) ||
                 (c.get_knight_occupancy(enemy_side) & MagicNumbers::KnightMoves[target_idx]) ||
                 (c.get_pawn_occupancy(enemy_side) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(move_side)) + target_idx]) ||
                 (c.get_king_occupancy(enemy_side) & MagicNumbers::KingMoves[target_idx]));
    } else [[likely]] {

        Bitboard checking_pieces = c.get_checkers(c.get_side_to_move());
        if (checking_pieces) {
            // if there's a piece checking our king - we know at most one piece can be checking as double checks are king moves only
            if (!(MagicNumbers::ConnectingSquares[(64 * king_idx) + bitboard_to_idx(checking_pieces)] & idx_to_bitboard(m.get_dest_square()))) {
                return false;
            }
        }

        if (idx_to_bitboard(m.get_src_square()) & c.get_pinned_pieces(c.get_side_to_move())) {
            return MagicNumbers::AlignedSquares[(64 * king_idx) + m.get_src_square()] & idx_to_bitboard(m.get_dest_square());
        }
    }

    return true;
}

bool MoveGenerator::is_move_pseudolegal(const ChessBoard& c, const Move to_test) {
    auto src_idx = to_test.get_src_square();
    auto moved_piece = c.get_piece(src_idx);
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

Bitboard MoveGenerator::generate_safe_king_spaces(const ChessBoard& c, const Side side) {
    Bitboard enemy_occupancy = c.get_occupancy(ENEMY_SIDE(side));
    const Bitboard blockers = (enemy_occupancy | c.get_occupancy(side)) ^ c.get_king_occupancy(side);
    Bitboard to_return = 0;
    while (enemy_occupancy) {
        const auto piece_idx = pop_min_bit(enemy_occupancy);
        const auto piece_type = c.get_piece(piece_idx).get_type();
        if (piece_type == PieceTypes::PAWN) {
            to_return |= MagicNumbers::PawnAttacks[(64 * static_cast<int>(ENEMY_SIDE(side))) + piece_idx];
        } else {
            to_return |= generate_movemask(piece_type, blockers, piece_idx);
        }
    }
    return ~to_return;
}