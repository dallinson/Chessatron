#include "move_generator.hpp"

#include <bit>
#include <immintrin.h>

#include "magic_numbers.hpp"

MoveList MoveGenerator::generate_legal_moves(ChessBoard& c, const int side) {
    MoveList to_return = generate_pseudolegal_moves(c, side);

    return filter_to_legal_moves(c, side, to_return);
}

MoveList MoveGenerator::generate_pseudolegal_moves(const ChessBoard& c, const int side) {
    MoveList to_return = MoveGenerator::generate_king_moves(c, side);

    int checking_piece_count = get_checking_piece_count(c, side, bitboard_to_idx(c.get_king_occupancy(side)));

    if (checking_piece_count >= 2) {
        return to_return;
    }

    if (checking_piece_count == 0) {
        to_return.add_moves(MoveGenerator::generate_castling_moves(c, side));
    }
    to_return.add_moves(MoveGenerator::generate_queen_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_bishop_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_knight_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_rook_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_pawn_moves(c, side));

    return to_return;
}

int MoveGenerator::get_checking_piece_count(const ChessBoard& c, const int side, const int king_idx) {
    Bitboard bishop_mask = MoveGenerator::generate_bishop_movemask(c.get_occupancy(), king_idx);
    Bitboard rook_mask = MoveGenerator::generate_rook_movemask(c.get_occupancy(), king_idx);

    int enemy_side = (side + 1) & 0x01;
    Bitboard enemy_queen_mask = c.get_queen_occupancy(enemy_side);

    int checking_pieces = std::popcount(enemy_queen_mask & (bishop_mask | rook_mask));
    checking_pieces += std::popcount(c.get_bishop_occupancy(enemy_side) & bishop_mask);
    checking_pieces += std::popcount(c.get_rook_occupancy(enemy_side) & rook_mask);
    // this calculates the checking pieces for the sliding pieces
    checking_pieces += std::popcount(c.get_knight_occupancy(enemy_side) & knightMoves[king_idx]);

    Bitboard pawns_attacking_here = pawnAttackMaps[(64 * side) + king_idx];
    // black is in the latter 64 spaces
    checking_pieces += std::popcount(c.get_pawn_occupancy(enemy_side) & pawns_attacking_here);

    return checking_pieces;
}

MoveList MoveGenerator::generate_king_moves(const ChessBoard& c, const int side) {
    int king_idx = bitboard_to_idx(c.get_king_occupancy(side));
    Bitboard king_moves = kingMoves[king_idx];
    return filter_to_pseudolegal_moves(c, side, king_moves, king_idx);
}

MoveList MoveGenerator::filter_to_pseudolegal_moves(const ChessBoard& c, const int side, const Bitboard potential_moves, const int idx) {
    MoveList to_return;
    Bitboard valid_moves = potential_moves & ~c.get_side_occupancy(side);
    // only move onto spaces unoccupied by friendlies
    int src_idx = idx & 0x3F;
    while (valid_moves) {
        int move_idx = pop_min_bit(&valid_moves);
        uint_fast16_t flags = 0;
        flags |= ((GET_BIT(c.get_side_occupancy((side + 1) & 1), move_idx)) << 2);

        to_return.add_move(Move((MoveFlags) flags, move_idx & 0x3F, src_idx));
    }
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

MoveList MoveGenerator::generate_queen_moves(const ChessBoard& c, const int side) {
    Bitboard queen_mask = c.get_queen_occupancy(side);
    MoveList to_return;
    while (queen_mask) {
        int queen_idx = pop_min_bit(&queen_mask);
        Bitboard queen_moves = MoveGenerator::generate_queen_movemask(c.get_occupancy(), queen_idx);
        to_return.add_moves(filter_to_pseudolegal_moves(c, side, queen_moves, queen_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_bishop_moves(const ChessBoard& c, const int side) {
    Bitboard bishop_mask = c.get_bishop_occupancy(side);
    MoveList to_return;
    while (bishop_mask) {
        int bishop_idx = pop_min_bit(&bishop_mask);
        Bitboard bishop_moves = MoveGenerator::generate_bishop_movemask(c.get_occupancy(), bishop_idx);
        to_return.add_moves(filter_to_pseudolegal_moves(c, side, bishop_moves, bishop_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_knight_moves(const ChessBoard& c, const int side) {
    Bitboard knight_mask = c.get_knight_occupancy(side);
    MoveList to_return;
    while (knight_mask) {
        int knight_idx = pop_min_bit(&knight_mask);
        Bitboard knight_moves = knightMoves[knight_idx];
        to_return.add_moves(filter_to_pseudolegal_moves(c, side, knight_moves, knight_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_rook_moves(const ChessBoard& c, const int side) {
    Bitboard rook_mask = c.get_rook_occupancy(side);
    MoveList to_return;
    while (rook_mask) {
        int rook_idx = pop_min_bit(&rook_mask);
        Bitboard rook_moves = MoveGenerator::generate_rook_movemask(c.get_occupancy(), rook_idx);
        to_return.add_moves(filter_to_pseudolegal_moves(c, side, rook_moves, rook_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_pawn_moves(const ChessBoard& c, const int side) {
    Bitboard pawn_mask = c.get_pawn_occupancy(side);
    MoveList to_return;
    while (pawn_mask) {
        int pawn_idx = pop_min_bit(&pawn_mask);
        if (side == 0) {
            if (!GET_BIT(c.get_occupancy(), pawn_idx + 8)) {
                if (GET_RANK(pawn_idx) == 1 && !GET_BIT(c.get_occupancy(), pawn_idx + 16)) {
                    to_return.add_move(Move(DOUBLE_PAWN_PUSH, pawn_idx + 16, pawn_idx));
                }
                if (GET_RANK(pawn_idx) == 6) {
                    to_return.add_move(Move(ROOK_PROMOTION, pawn_idx + 8, pawn_idx));
                    to_return.add_move(Move(KNIGHT_PROMOTION, pawn_idx + 8, pawn_idx));
                    to_return.add_move(Move(BISHOP_PROMOTION, pawn_idx + 8, pawn_idx));
                    to_return.add_move(Move(QUEEN_PROMOTION, pawn_idx + 8, pawn_idx));
                } else {
                    to_return.add_move(Move(QUIET_MOVE, pawn_idx + 8, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 0 && GET_BIT(c.get_side_occupancy((side + 1) & 1), pawn_idx + 7)) {
                if (GET_RANK(pawn_idx) == 6) {
                    to_return.add_move(Move(ROOK_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                    to_return.add_move(Move(KNIGHT_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                    to_return.add_move(Move(BISHOP_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                    to_return.add_move(Move(QUEEN_PROMOTION_CAPTURE, pawn_idx + 7, pawn_idx));
                } else {
                    to_return.add_move(Move(CAPTURE, pawn_idx + 7, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 7 && GET_BIT(c.get_side_occupancy((side + 1) & 1), pawn_idx + 9)) {
                if (GET_RANK(pawn_idx) == 6) {
                    to_return.add_move(Move(ROOK_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                    to_return.add_move(Move(KNIGHT_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                    to_return.add_move(Move(BISHOP_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                    to_return.add_move(Move(QUEEN_PROMOTION_CAPTURE, pawn_idx + 9, pawn_idx));
                } else {
                    to_return.add_move(Move(CAPTURE, pawn_idx + 9, pawn_idx));
                }
            }
            if (GET_RANK(pawn_idx) == 4 && (std::abs(((int) c.get_en_passant_file()) - (int) GET_FILE(pawn_idx)) == 1)) {
                to_return.add_move(Move(EN_PASSANT_CAPTURE, POSITION(5, c.get_en_passant_file()), pawn_idx));
            }
        } else {
            if (!GET_BIT(c.get_occupancy(), pawn_idx - 8)) {
                if (GET_RANK(pawn_idx) == 6 && !GET_BIT(c.get_occupancy(), pawn_idx - 16)) {
                    to_return.add_move(Move(DOUBLE_PAWN_PUSH, pawn_idx - 16, pawn_idx));
                }
                if (GET_RANK(pawn_idx) == 1) {
                    to_return.add_move(Move(ROOK_PROMOTION, pawn_idx - 8, pawn_idx));
                    to_return.add_move(Move(KNIGHT_PROMOTION, pawn_idx - 8, pawn_idx));
                    to_return.add_move(Move(BISHOP_PROMOTION, pawn_idx - 8, pawn_idx));
                    to_return.add_move(Move(QUEEN_PROMOTION, pawn_idx - 8, pawn_idx));
                } else {
                    to_return.add_move(Move(QUIET_MOVE, pawn_idx - 8, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 0 && GET_BIT(c.get_side_occupancy((side + 1) & 1), pawn_idx - 9)) {
                if (GET_RANK(pawn_idx) == 1) {
                    to_return.add_move(Move(ROOK_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                    to_return.add_move(Move(KNIGHT_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                    to_return.add_move(Move(BISHOP_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                    to_return.add_move(Move(QUEEN_PROMOTION_CAPTURE, pawn_idx - 9, pawn_idx));
                } else {
                    to_return.add_move(Move(CAPTURE, pawn_idx - 9, pawn_idx));
                }
            }
            if (GET_FILE(pawn_idx) != 7 && GET_BIT(c.get_side_occupancy((side + 1) & 1), pawn_idx - 7)) {
                if (GET_RANK(pawn_idx) == 1) {
                    to_return.add_move(Move(ROOK_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                    to_return.add_move(Move(KNIGHT_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                    to_return.add_move(Move(BISHOP_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                    to_return.add_move(Move(QUEEN_PROMOTION_CAPTURE, pawn_idx - 7, pawn_idx));
                } else {
                    to_return.add_move(Move(CAPTURE, pawn_idx - 7, pawn_idx));
                }
            }
            if (GET_RANK(pawn_idx) == 3 && (std::abs(((int) c.get_en_passant_file()) - (int) GET_FILE(pawn_idx)) == 1)) {
                to_return.add_move(Move(EN_PASSANT_CAPTURE, POSITION(2, c.get_en_passant_file()), pawn_idx));
            }
        }
    }

    return to_return;
}

MoveList MoveGenerator::generate_castling_moves(const ChessBoard& c, const int side) {
    MoveList to_return;
    if (c.get_kingside_castling(side)) {
        int shift_val = 56 * side;
        if (((uint64_t) 0b10010000 ^ ((c.get_occupancy() >> shift_val) & 0xF0)) == 0) {
            // if only these spaces are occupied
            if (get_checking_piece_count(c, side, 5 + shift_val) == 0) {
                to_return.add_move(Move(KINGSIDE_CASTLE, 6 + shift_val, 4 + shift_val));
            }
        }
    }
    if (c.get_queenside_castling(side)) {
        int shift_val = 56 * side;
        if (((uint64_t) 0b00010001 ^ ((c.get_occupancy() >> shift_val) & 0x1F)) == 0) {
            // if only these spaces are occupied
            if (get_checking_piece_count(c, side, 3 + shift_val) == 0) {
                to_return.add_move(Move(QUEENSIDE_CASTLE, 2 + shift_val, 4 + shift_val));
            }
        }
    }

    return to_return;
}

MoveList MoveGenerator::filter_to_legal_moves(ChessBoard& c, const int side, const MoveList& move_list) {
    MoveList to_return;
    MoveHistory history;
    for (size_t i = 0; i < move_list.len(); i++) {
        if (!is_move_legal(c, move_list[i])) {
            continue;
        }
        c.make_move(move_list[i], history);
        if (get_checking_piece_count(c, side, bitboard_to_idx(c.get_king_occupancy(side))) == 0) {
            to_return.add_move(move_list[i]);
        }
        c.unmake_move(history);
    }
    return to_return;
}

bool MoveGenerator::is_move_legal(const ChessBoard& c, const Move m) {
    int king_idx = bitboard_to_idx(c.get_king_occupancy(c.get_side()));
    if (m.get_move_flags() == EN_PASSANT_CAPTURE) {
        // with en passant knights and pawns _cannot_ capture as the previous
        // move was moving a pawn, and therefore knights/pawns were not in
        // position to capture-they'd have needed to be moved two moves ago,
        // and we would have moved out of check
        Bitboard occupancy = c.get_occupancy();
        Bitboard cleared_occupancy =
            occupancy ^ (idx_to_bitboard(m.get_src_square()) | idx_to_bitboard(m.get_dest_square() - 8 + (16 * c.get_side())));
        // clear the origin and capture spaces
        // then set the destination square
        cleared_occupancy |= idx_to_bitboard(m.get_dest_square());
        int enemy_side = (c.get_side() + 1) & 1;
        return !((MoveGenerator::generate_bishop_movemask(cleared_occupancy, king_idx) &
                  (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
                 (MoveGenerator::generate_rook_movemask(cleared_occupancy, king_idx) &
                  (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))));
    }

    if (c.get_piece(m.get_src_square()).get_type() == KING_VALUE) {
        Bitboard cleared_bitboard = c.get_occupancy() ^ idx_to_bitboard(m.get_src_square());
        int target_idx = m.get_dest_square();
        int enemy_side = (c.get_piece(m.get_src_square()).get_side() + 1) & 1;
        return !(
            (generate_bishop_movemask(cleared_bitboard, target_idx) & (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
            (generate_rook_movemask(cleared_bitboard, target_idx) & (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
            (c.get_knight_occupancy(enemy_side) & knightMoves[target_idx]) ||
            (c.get_pawn_occupancy(enemy_side) & pawnAttackMaps[(64 * c.get_piece(m.get_src_square()).get_side()) + target_idx]));
    }

    return true;
}