#include "move_generator.hpp"

#include <bit>

#include "magic_numbers.hpp"

MoveList MoveGenerator::generate_legal_moves(const ChessBoard& c, const Side side) {
    MoveList to_return = generate_pseudolegal_moves(c, side);

    //return filter_to_legal_moves(c, to_return);
    return to_return;
}

MoveList MoveGenerator::generate_pseudolegal_moves(const ChessBoard& c, const Side side) {
    MoveList to_return;

    MoveGenerator::generate_moves<PieceTypes::KING>(c, side, to_return);

    int checking_piece_count = MoveGenerator::get_checking_piece_count(c, side, bitboard_to_idx(c.get_king_occupancy(side)));

    if (checking_piece_count >= 2) {
        return to_return;
    }

    if (checking_piece_count == 0) {
        MoveGenerator::generate_castling_moves(c, side, to_return);
    }
    MoveGenerator::generate_moves<PieceTypes::QUEEN>(c, side, to_return);
    MoveGenerator::generate_moves<PieceTypes::BISHOP>(c, side, to_return);
    MoveGenerator::generate_moves<PieceTypes::KNIGHT>(c, side, to_return);
    MoveGenerator::generate_moves<PieceTypes::ROOK>(c, side, to_return);
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

void MoveGenerator::filter_to_pseudolegal_moves(const Bitboard friendlies, const Bitboard enemies, const Bitboard potential_moves, const int idx,
                                                MoveList& move_list) {
    Bitboard valid_moves = potential_moves & ~friendlies;
    // only move onto spaces unoccupied by friendlies
    int src_idx = idx & 0x3F;
    while (valid_moves) {
        int move_idx = pop_min_bit(valid_moves);
        uint16_t flags = 0;
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
    default:
        break;
    }
    return 0;
}

template <PieceTypes piece_type> void MoveGenerator::generate_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    if constexpr (piece_type == PieceTypes::PAWN) {
        return generate_pawn_moves(c, side, move_list);
    }
    const int king_idx = bitboard_to_idx(c.get_king_occupancy(side));
    const Bitboard friendly_occupancy = c.get_side_occupancy(side);
    const Bitboard total_occupancy = c.get_side_occupancy(ENEMY_SIDE(side)) | friendly_occupancy;
    const auto checking_idx = bitboard_to_idx(c.get_checkers(side));
    const auto enemy_side = ENEMY_SIDE(side);
    Bitboard pieces = c.get_piece_occupancy<piece_type>(side);
    while (pieces) {
        const auto piece_idx = pop_min_bit(pieces);
        auto potential_moves = generate_movemask(piece_type, total_occupancy, piece_idx) & ~friendly_occupancy;
        if constexpr (piece_type != PieceTypes::KING) {
            if (checking_idx != 64) {
                // no checking pieces implies the checking idx is 64
                potential_moves &= MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx];
            } 
            if ((idx_to_bitboard(piece_idx) & c.get_pinned_pieces(side)) != 0) {
                // if we are pinned
                potential_moves &= MagicNumbers::AlignedSquares[(64 * king_idx) + piece_idx];
            }
            // these are the only valid move positions
        }

        while (potential_moves) {
            const auto target_idx = pop_min_bit(potential_moves);
            if constexpr (piece_type == PieceTypes::KING) {
                const Bitboard cleared_bitboard = total_occupancy ^ idx_to_bitboard(king_idx) ^ idx_to_bitboard(target_idx);
                const bool is_dest_checked =
                    ((generate_bishop_movemask(cleared_bitboard, target_idx) &
                      (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
                     (generate_rook_movemask(cleared_bitboard, target_idx) &
                      (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side))) ||
                     (c.get_knight_occupancy(enemy_side) & MagicNumbers::KnightMoves[target_idx]) ||
                     (c.get_pawn_occupancy(enemy_side) & MagicNumbers::PawnAttacks[(64 * static_cast<int>(side)) + target_idx]) ||
                     (c.get_king_occupancy(enemy_side) & MagicNumbers::KingMoves[target_idx]));
                if (is_dest_checked) {
                    continue;
                }
            }
            if (c.get_piece(target_idx).get_value() != 0) {
                // is a capture
                move_list.add_move(Move(MoveFlags::CAPTURE, target_idx, piece_idx));
            } else {
                move_list.add_move(Move(MoveFlags::QUIET_MOVE, target_idx, piece_idx));
            }
        }
    }
}

void MoveGenerator::generate_pawn_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard pawn_mask = c.get_pawn_occupancy(side);
    const Side enemy_side = ENEMY_SIDE(side);
    const Bitboard enemy_occupancy = c.get_occupancy(enemy_side);
    const Bitboard total_occupancy = enemy_occupancy | c.get_occupancy(side);
    const auto king_idx = bitboard_to_idx(c.get_king_occupancy(side));
    const auto ahead_offset = side == Side::WHITE ? 8 : -8;
    const auto capture_front_left = side == Side::WHITE ? 7 : -7;
    const auto capture_front_right = side == Side::WHITE ? 9 : -9;
    const auto left_wall = side == Side::WHITE ? 0u : 7u;
    const auto right_wall = side == Side::WHITE ? 7u : 0u;

    const auto checking_idx = bitboard_to_idx(c.get_checkers(side));

    const auto start_rank = side == Side::WHITE ? 1u : 6u;
    const auto penultimate_rank = side == Side::WHITE ? 6u : 1u;
    const auto ep_rank = side == Side::WHITE ? 4u : 3u;
    while (pawn_mask) {
        int pawn_idx = pop_min_bit(pawn_mask);
        if (GET_BIT(total_occupancy, pawn_idx + ahead_offset) == 0 &&
            (GET_BIT(c.get_pinned_pieces(side), pawn_idx) == 0 || GET_FILE(pawn_idx) == GET_FILE(king_idx))) {
            // if not pinned, or the pawn motion is aligned with the king
            if (checking_idx == 64 ||
                (idx_to_bitboard(pawn_idx + ahead_offset) & MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx]) != 0) {
                // if not in check or can block

                if (GET_RANK(pawn_idx) == penultimate_rank) {
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::QUIET_MOVE, pawn_idx + ahead_offset, pawn_idx));
                }
            }
            if ((GET_RANK(pawn_idx) == start_rank && GET_BIT(total_occupancy, pawn_idx + ahead_offset + ahead_offset) == 0) &&
                (checking_idx == 64 ||
                 (idx_to_bitboard(pawn_idx + ahead_offset + ahead_offset) & MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx]) != 0)) {
                move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, pawn_idx + ahead_offset + ahead_offset, pawn_idx));
            }
        }
        // This handles advancing

        if ((GET_FILE(pawn_idx) != left_wall && GET_BIT(enemy_occupancy, pawn_idx + capture_front_left)) &&
            // if there is a piece we _can_ capture
            (GET_BIT(c.get_pinned_pieces(side), pawn_idx) == 0 || is_aligned(king_idx, pawn_idx, pawn_idx + capture_front_left)) &&
            // and we're not pinned/are moving in the capture direction
            (checking_idx == 64 || checking_idx == pawn_idx + capture_front_left)) {
            // or we want are capturing the checking piece
            if (GET_RANK(pawn_idx) != penultimate_rank) {
                move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + capture_front_left, pawn_idx));
            } else {
                move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
            }
        }

        if ((GET_FILE(pawn_idx) != right_wall && GET_BIT(enemy_occupancy, pawn_idx + capture_front_right)) &&
            // if there is a piece we _can_ capture
            (GET_BIT(c.get_pinned_pieces(side), pawn_idx) == 0 || is_aligned(king_idx, pawn_idx, pawn_idx + capture_front_right)) &&
            // and we're not pinned/are moving in the capture direction
            (checking_idx == 64 || checking_idx == pawn_idx + capture_front_right)) {
            // or we want are capturing the checking piece
            if (GET_RANK(pawn_idx) != penultimate_rank) {
                move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + capture_front_right, pawn_idx));
            } else {
                move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
            }
        }

        if (GET_RANK(pawn_idx) == ep_rank && std::abs(c.get_en_passant_file() - static_cast<int>(GET_FILE(pawn_idx))) == 1) {
            // if a pseudolegal en passant is possible
            const auto ep_target_square = POSITION(((int) GET_RANK(pawn_idx)) + (ahead_offset / 8), c.get_en_passant_file());
            const Bitboard cleared_bitboard = total_occupancy ^ idx_to_bitboard(pawn_idx) ^ idx_to_bitboard(ep_target_square) ^ idx_to_bitboard(ep_target_square  - ahead_offset);
            const Bitboard threatening_bishops = generate_bishop_movemask(cleared_bitboard, king_idx) & (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));
            const Bitboard threatening_rooks = generate_rook_movemask(cleared_bitboard, king_idx) & (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));

            if (threatening_bishops == 0 && threatening_rooks == 0) {
                move_list.add_move(Move(MoveFlags::EN_PASSANT_CAPTURE, ep_target_square, pawn_idx));
            }
        }
    }
}

void MoveGenerator::generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    if (c.get_kingside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b10010000 ^ ((c.get_occupancy() >> shift_val) & 0xF0)) == 0) {
            // if only these spaces are occupied
            if (get_checking_piece_count(c, side, 5 + shift_val) == 0 && get_checking_piece_count(c, side, 6 + shift_val) == 0) {
                move_list.add_move(Move(MoveFlags::KINGSIDE_CASTLE, 6 + shift_val, 4 + shift_val));
            }
        }
    }
    if (c.get_queenside_castling(side)) {
        int shift_val = 56 * static_cast<int>(side);
        if (((uint64_t) 0b00010001 ^ ((c.get_occupancy() >> shift_val) & 0x1F)) == 0) {
            // if only these spaces are occupied
            if (get_checking_piece_count(c, side, 3 + shift_val) == 0 && get_checking_piece_count(c, side, 2 + shift_val) == 0) {
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
        generate_pawn_moves(c, move_side, moves);
        break;
    case PieceTypes::KNIGHT:
        generate_moves<PieceTypes::KNIGHT>(c, move_side, moves);
        break;
    case PieceTypes::BISHOP:
        generate_moves<PieceTypes::BISHOP>(c, move_side, moves);
        break;
    case PieceTypes::ROOK:
        generate_moves<PieceTypes::ROOK>(c, move_side, moves);
        break;
    case PieceTypes::QUEEN:
        generate_moves<PieceTypes::QUEEN>(c, move_side, moves);
        break;
    case PieceTypes::KING:
        generate_moves<PieceTypes::KING>(c, move_side, moves);
        break;
    }

    for (size_t i = 0; i < moves.len(); i++) {
        if (moves[i] == to_test) {
            return true;
        }
    }
    return false;
}