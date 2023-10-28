#include "move_generator.hpp"

#include <bit>

#include "magic_numbers.hpp"

MoveList MoveGenerator::generate_legal_moves(const ChessBoard& c, const Side side) {
    MoveList to_return = generate_pseudolegal_moves(c, side);

    return filter_to_legal_moves(c, to_return);
}

MoveList MoveGenerator::generate_pseudolegal_moves(const ChessBoard& c, const Side side) {
    MoveList to_return;
    const Bitboard side_occupancy = c.get_side_occupancy(side);
    const Bitboard enemy_side_occupancy = c.get_side_occupancy(ENEMY_SIDE(side));

    MoveGenerator::generate_king_moves(side_occupancy, enemy_side_occupancy, bitboard_to_idx(c.get_king_occupancy(side)), to_return);

    int checking_piece_count = MoveGenerator::get_checking_piece_count(c, side, bitboard_to_idx(c.get_king_occupancy(side)));

    if (checking_piece_count >= 2) {
        return to_return;
    }

    if (checking_piece_count == 0) {
        MoveGenerator::generate_castling_moves(c, side, to_return);
    }
    MoveGenerator::generate_queen_moves(c, side, to_return);
    MoveGenerator::generate_bishop_moves(c, side, to_return);
    MoveGenerator::generate_knight_moves(c, side, to_return);
    MoveGenerator::generate_rook_moves(c, side, to_return);
    MoveGenerator::generate_pawn_moves(c, side, to_return);

    return to_return;
}

int MoveGenerator::get_checking_piece_count(const ChessBoard& c, const Side side, const int king_idx) {
    return std::popcount(MoveGenerator::get_checkers(c, side, king_idx));
}

Bitboard MoveGenerator::get_checkers(const ChessBoard& c, const Side side, const int king_idx) {
    const Side enemy_side = ENEMY_SIDE(side);

    Bitboard bishop_mask = MoveGenerator::generate_bishop_movemask(c.get_occupancy(), king_idx);
    Bitboard rook_mask = MoveGenerator::generate_rook_movemask(c.get_occupancy(), king_idx);

    Bitboard to_return = 0;

    to_return |= c.get_queen_occupancy(enemy_side) & (bishop_mask | rook_mask);
    to_return |= c.get_bishop_occupancy(enemy_side) & bishop_mask;
    to_return |= c.get_rook_occupancy(enemy_side) & rook_mask;
    to_return |= c.get_knight_occupancy(enemy_side) & MagicNumbers::KnightMoves[king_idx];
    to_return |= c.get_pawn_occupancy(enemy_side) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(side)) + king_idx];

    return to_return;
}

void MoveGenerator::generate_king_moves(const Bitboard friendlies, const Bitboard enemies, const int king_idx, MoveList& move_list) {
    Bitboard king_moves = MagicNumbers::KingMoves[king_idx];
    filter_to_pseudolegal_moves(friendlies, enemies, king_moves, king_idx, move_list);
}

void MoveGenerator::filter_to_pseudolegal_moves(const Bitboard friendlies, const Bitboard enemies, const Bitboard potential_moves, const int idx,
                                                MoveList& move_list) {
    Bitboard valid_moves = potential_moves & ~friendlies;
    // only move onto spaces unoccupied by friendlies
    int src_idx = idx & 0x3F;
    while (valid_moves) {
        int move_idx = pop_min_bit(valid_moves);
        uint_fast16_t flags = 0;
        flags |= ((GET_BIT(enemies, move_idx)) << 2);

        move_list.add_move(Move((MoveFlags) flags, move_idx & 0x3F, src_idx));
    }
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

void MoveGenerator::generate_queen_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard queen_mask = c.get_queen_occupancy(side);
    const Bitboard friendlies = c.get_side_occupancy(side);
    const Bitboard enemies = c.get_side_occupancy(ENEMY_SIDE(side));
    const Bitboard total_occupancy = friendlies | enemies;
    while (queen_mask) {
        int queen_idx = pop_min_bit(queen_mask);
        Bitboard queen_moves = MoveGenerator::generate_queen_movemask(total_occupancy, queen_idx);
        filter_to_pseudolegal_moves(friendlies, enemies, queen_moves, queen_idx, move_list);
    }
}

void MoveGenerator::generate_bishop_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard bishop_mask = c.get_bishop_occupancy(side);
    const Bitboard friendlies = c.get_side_occupancy(side);
    const Bitboard enemies = c.get_side_occupancy(ENEMY_SIDE(side));
    const Bitboard total_occupancy = friendlies | enemies;
    while (bishop_mask) {
        int bishop_idx = pop_min_bit(bishop_mask);
        Bitboard bishop_moves = MoveGenerator::generate_bishop_movemask(total_occupancy, bishop_idx);
        filter_to_pseudolegal_moves(friendlies, enemies, bishop_moves, bishop_idx, move_list);
    }
}

void MoveGenerator::generate_knight_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard knight_mask = c.get_knight_occupancy(side);
    const Bitboard friendlies = c.get_side_occupancy(side);
    const Bitboard enemies = c.get_side_occupancy(ENEMY_SIDE(side));
    while (knight_mask) {
        int knight_idx = pop_min_bit(knight_mask);
        Bitboard knight_moves = MagicNumbers::KnightMoves[knight_idx];
        filter_to_pseudolegal_moves(friendlies, enemies, knight_moves, knight_idx, move_list);
    }
}

void MoveGenerator::generate_rook_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard rook_mask = c.get_rook_occupancy(side);
    const Bitboard friendlies = c.get_side_occupancy(side);
    const Bitboard enemies = c.get_side_occupancy(ENEMY_SIDE(side));
    const Bitboard total_occupancy = friendlies | enemies;
    while (rook_mask) {
        int rook_idx = pop_min_bit(rook_mask);
        Bitboard rook_moves = MoveGenerator::generate_rook_movemask(total_occupancy, rook_idx);
        filter_to_pseudolegal_moves(friendlies, enemies, rook_moves, rook_idx, move_list);
    }
}

void MoveGenerator::generate_pawn_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard pawn_mask = c.get_pawn_occupancy(side);
    const Side enemy_side = ENEMY_SIDE(side);
    const Bitboard enemy_occupancy = c.get_occupancy(enemy_side);
    const Bitboard total_occupancy = enemy_occupancy | c.get_occupancy(side);
    while (pawn_mask) {
        int pawn_idx = pop_min_bit(pawn_mask);
        if (side == Side::WHITE) {
            if (!GET_BIT(total_occupancy, pawn_idx + 8)) {
                if (GET_RANK(pawn_idx) == 1 && !GET_BIT(total_occupancy, pawn_idx + 16)) {
                    move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, pawn_idx + 16, pawn_idx));
                }
                if (GET_RANK(pawn_idx) == 6) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION, pawn_idx + 8, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION, pawn_idx + 8, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION, pawn_idx + 8, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION, pawn_idx + 8, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::QUIET_MOVE, pawn_idx + 8, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 0 && GET_BIT(enemy_occupancy, pawn_idx + 7)) {
                if (GET_RANK(pawn_idx) == 6) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + 7, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 7 && GET_BIT(enemy_occupancy, pawn_idx + 9)) {
                if (GET_RANK(pawn_idx) == 6) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + 9, pawn_idx));
                }
            }
            if (GET_RANK(pawn_idx) == 4 && (std::abs(((int) c.get_en_passant_file()) - (int) GET_FILE(pawn_idx)) == 1)) {
                move_list.add_move(Move(MoveFlags::EN_PASSANT_CAPTURE, POSITION(5, c.get_en_passant_file()), pawn_idx));
            }
        } else {
            if (!GET_BIT(total_occupancy, pawn_idx - 8)) {
                if (GET_RANK(pawn_idx) == 6 && !GET_BIT(total_occupancy, pawn_idx - 16)) {
                    move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, pawn_idx - 16, pawn_idx));
                }
                if (GET_RANK(pawn_idx) == 1) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION, pawn_idx - 8, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION, pawn_idx - 8, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION, pawn_idx - 8, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION, pawn_idx - 8, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::QUIET_MOVE, pawn_idx - 8, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 0 && GET_BIT(enemy_occupancy, pawn_idx - 9)) {
                if (GET_RANK(pawn_idx) == 1) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx - 9, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 7 && GET_BIT(enemy_occupancy, pawn_idx - 7)) {
                if (GET_RANK(pawn_idx) == 1) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx - 7, pawn_idx));
                }
            }
            if (GET_RANK(pawn_idx) == 3 && (std::abs(((int) c.get_en_passant_file()) - (int) GET_FILE(pawn_idx)) == 1)) {
                move_list.add_move(Move(MoveFlags::EN_PASSANT_CAPTURE, POSITION(2, c.get_en_passant_file()), pawn_idx));
            }
        }
    }
}

void MoveGenerator::generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    if (c.get_kingside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b10010000 ^ ((c.get_occupancy() >> shift_val) & 0xF0)) == 0) {
            // if only these spaces are occupied
            if (get_checking_piece_count(c, side, 5 + shift_val) == 0) {
                move_list.add_move(Move(MoveFlags::KINGSIDE_CASTLE, 6 + shift_val, 4 + shift_val));
            }
        }
    }
    if (c.get_queenside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b00010001 ^ ((c.get_occupancy() >> shift_val) & 0x1F)) == 0) {
            // if only these spaces are occupied
            if (get_checking_piece_count(c, side, 3 + shift_val) == 0) {
                move_list.add_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2 + shift_val, 4 + shift_val));
            }
        }
    }
}

MoveList MoveGenerator::filter_to_legal_moves(const ChessBoard& c, const MoveList& move_list) {
    MoveList to_return;
    for (size_t i = 0; i < move_list.len(); i++) {
        if (is_move_legal(c, move_list[i])) {
            to_return.add_move(move_list[i]);
        }
    }
    return to_return;
}

bool MoveGenerator::is_move_legal(const ChessBoard& c, const Move m) {
    int king_idx = bitboard_to_idx(c.get_king_occupancy(c.get_side_to_move()));
    const Side enemy_side = ENEMY_SIDE(c.get_piece(m.get_src_square()).get_side());
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
        return !(
            (generate_bishop_movemask(cleared_bitboard, target_idx) & (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
            (generate_rook_movemask(cleared_bitboard, target_idx) & (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
            (c.get_knight_occupancy(enemy_side) & MagicNumbers::KnightMoves[target_idx]) ||
            (c.get_pawn_occupancy(enemy_side) &
             MagicNumbers::PawnAttacks[(64 * static_cast<int>(c.get_piece(m.get_src_square()).get_side())) + target_idx]) ||
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