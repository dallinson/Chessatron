#pragma once

#include <cstdint>

#include "chessboard.hpp"
#include "move.hpp"
#include "utils.hpp"

enum class MoveGenType {
    ALL_LEGAL,
    CAPTURES,
    NON_CAPTURES
};

namespace MoveGenerator {
    template <MoveGenType gen_type> MoveList generate_legal_moves(const ChessBoard& c, const Side side);
    int get_checking_piece_count(const ChessBoard& c, const Side side);
    Bitboard get_checkers(const ChessBoard& c, const Side side);
    Bitboard get_attackers(const ChessBoard& board, const Side side, const int target_idx, const Bitboard occupancy);

    Bitboard generate_bishop_movemask(const Bitboard b, const int idx);
    Bitboard generate_rook_movemask(const Bitboard b, const int idx);
    Bitboard generate_queen_movemask(const Bitboard b, const int idx);
    template <PieceTypes piece_type> Bitboard generate_movemask(const Bitboard b, const int idx);

    template <PieceTypes piece_type, MoveGenType gen_type> void generate_moves(const ChessBoard& c, const Side side, MoveList& move_list);
    template <MoveGenType gen_type> void generate_pawn_moves(const ChessBoard& c, const Side side, MoveList& move_list);
    void generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list);

    bool is_move_legal(const ChessBoard& c, const Move m);
    bool is_move_pseudolegal(const ChessBoard& c, const Move to_test);

    template <MoveGenType gen_type> MoveList generate_legal_moves(const ChessBoard& c, const Side side);

} // namespace MoveGenerator

template <MoveGenType gen_type> MoveList MoveGenerator::generate_legal_moves(const ChessBoard& c, const Side side) {
    MoveList to_return;

    MoveGenerator::generate_moves<PieceTypes::KING, gen_type>(c, side, to_return);

    int checking_piece_count = MoveGenerator::get_checking_piece_count(c, side);

    if (checking_piece_count >= 2) {
        return to_return;
    }

    if (gen_type != MoveGenType::CAPTURES && checking_piece_count == 0) {
        MoveGenerator::generate_castling_moves(c, side, to_return);
    }
    MoveGenerator::generate_moves<PieceTypes::QUEEN, gen_type>(c, side, to_return);
    MoveGenerator::generate_moves<PieceTypes::BISHOP, gen_type>(c, side, to_return);
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, gen_type>(c, side, to_return);
    MoveGenerator::generate_moves<PieceTypes::ROOK, gen_type>(c, side, to_return);
    MoveGenerator::generate_pawn_moves<gen_type>(c, side, to_return);

    return to_return;
}

template <>
inline Bitboard MoveGenerator::generate_movemask<PieceTypes::BISHOP>(const Bitboard b, const int idx) {
    return generate_bishop_movemask(b, idx);
}

template <>
inline Bitboard MoveGenerator::generate_movemask<PieceTypes::ROOK>(const Bitboard b, const int idx) {
    return generate_rook_movemask(b, idx);
}

template <>
inline Bitboard MoveGenerator::generate_movemask<PieceTypes::QUEEN>(const Bitboard b, const int idx) {
    return generate_queen_movemask(b, idx);
}

template <>
inline Bitboard MoveGenerator::generate_movemask<PieceTypes::KNIGHT>(const Bitboard b, const int idx) {
    (void) b;
    (void) idx;
    return MagicNumbers::KnightMoves[idx];
}

template <>
inline Bitboard MoveGenerator::generate_movemask<PieceTypes::KING>(const Bitboard b, const int idx) {
    (void) b;
    (void) idx;
    return MagicNumbers::KingMoves[idx];
}


template <PieceTypes piece_type, MoveGenType gen_type> void MoveGenerator::generate_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    if constexpr (piece_type == PieceTypes::PAWN) {
        return generate_pawn_moves<gen_type>(c, side, move_list);
    }
    const int king_idx = get_lsb(c.get_king_occupancy(side));
    const Bitboard friendly_occupancy = c.get_side_occupancy(side);
    const Bitboard enemy_occupancy = c.get_side_occupancy(ENEMY_SIDE(side));
    const Bitboard total_occupancy = enemy_occupancy | friendly_occupancy;
    const auto checking_idx = get_lsb(c.get_checkers(side));
    const auto enemy_side = ENEMY_SIDE(side);
    Bitboard pieces = c.get_piece_occupancy<piece_type>(side);
    while (pieces) {
        const auto piece_idx = pop_min_bit(pieces);
        auto potential_moves = generate_movemask<piece_type>(total_occupancy, piece_idx) & ~friendly_occupancy;
        if constexpr (gen_type == MoveGenType::CAPTURES) {
            potential_moves &= enemy_occupancy;
        } else if constexpr (gen_type == MoveGenType::NON_CAPTURES) {
            potential_moves &= ~enemy_occupancy;
        }
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
                const Bitboard cleared_bitboard = total_occupancy ^ idx_to_bitboard(king_idx);
                const auto potential_diagonal_sliders = (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));
                const auto potential_orthogonal_sliders = (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));
                const bool is_dest_checked =
                    ((generate_bishop_movemask(cleared_bitboard, target_idx) & potential_diagonal_sliders) ||
                     (generate_rook_movemask(cleared_bitboard, target_idx) & potential_orthogonal_sliders) ||
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

template <MoveGenType gen_type> void MoveGenerator::generate_pawn_moves(const ChessBoard& board, const Side side, MoveList& move_list) {
    const Bitboard pawn_mask = board.get_pawn_occupancy(side);
    const Bitboard unpinned_pawns = pawn_mask & ~board.get_pinned_pieces(side);
    const Bitboard pinned_pawns = pawn_mask & board.get_pinned_pieces(side);

    const Side enemy_side = ENEMY_SIDE(side);
    const Bitboard enemy_occupancy = board.get_occupancy(enemy_side);
    const Bitboard total_occupancy = enemy_occupancy | board.get_occupancy(side);

    const auto king_idx = get_lsb(board.get_king_occupancy(side));
    const int ahead_offset = side == Side::WHITE ? 8 : -8;

    const auto checking_idx = get_lsb(board.get_checkers(side));
    const auto in_check = board.get_checkers(side) != 0;

    const Bitboard valid_in_check_moves = in_check ? MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx] : 0xFFFFFFFFFFFFFFFF;

    constexpr Bitboard rank_bitboard = 0x00000000000000FF;
    constexpr Bitboard file_bitboard = 0x101010101010101;
    constexpr Bitboard a_file = file_bitboard;
    constexpr Bitboard h_file = file_bitboard << 7;

    const auto start_rank = side == Side::WHITE ? 1u : 6u;
    const auto start_rank_bitboard = rank_bitboard << (start_rank * 8);
    const auto penultimate_rank = side == Side::WHITE ? 6u : 1u;
    const auto penultimate_rank_bitboard = rank_bitboard << (penultimate_rank * 8);
    const auto ep_rank = side == Side::WHITE ? 4u : 3u;
    const auto ep_rank_bitboard = rank_bitboard << (ep_rank * 8);

    
    // First we examine unpinned pieces
    {
        Bitboard advancing_board = (side == Side::WHITE ? unpinned_pawns << 8 : unpinned_pawns >> 8) & ~total_occupancy;
        Bitboard double_advancing_board = (side == Side::WHITE ? advancing_board << 8 : advancing_board >> 8) & ~total_occupancy & valid_in_check_moves;
        advancing_board &= valid_in_check_moves;
        advancing_board = side == Side::WHITE ? advancing_board >> 8 : advancing_board << 8;
        double_advancing_board = side == Side::WHITE ? double_advancing_board >> 16 : double_advancing_board << 16;
        Bitboard promotables = advancing_board & penultimate_rank_bitboard;
        advancing_board &= ~penultimate_rank_bitboard;
        double_advancing_board &= start_rank_bitboard;
        while (advancing_board) {
            const auto pawn_idx = pop_min_bit(advancing_board);
            move_list.add_move(Move(MoveFlags::QUIET_MOVE, pawn_idx + ahead_offset, pawn_idx));
        }
        while (promotables) {
            const auto pawn_idx = pop_min_bit(promotables);
            move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
            move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
            move_list.add_move(Move(MoveFlags::ROOK_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
            move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
        }
        while (double_advancing_board) {
            const auto pawn_idx = pop_min_bit(double_advancing_board);
            move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, pawn_idx + (2 *ahead_offset), pawn_idx));
        }
        // Next we examine captures - 0 is h-side, 1 is a-side
        Bitboard potential_captures[2];
        Bitboard promotion_captures[2];
        int shifts[2];
        potential_captures[0] = (side == Side::WHITE ? unpinned_pawns << 9 : unpinned_pawns >> 7) & enemy_occupancy & ~a_file & valid_in_check_moves;
        potential_captures[0] = side == Side::WHITE ? potential_captures[0] >> 9 : potential_captures[0] << 7;
        potential_captures[1] = (side == Side::WHITE ? unpinned_pawns << 7 : unpinned_pawns >> 9) & enemy_occupancy & ~h_file & valid_in_check_moves;
        potential_captures[1] = side == Side::WHITE ? potential_captures[1] >> 7 : potential_captures[1] << 9;

        shifts[0] = side == Side::WHITE ? 9 : -7;
        shifts[1] = side == Side::WHITE ? 7 : -9;

        for (int i = 0; i < 2; i++) {
            promotion_captures[i] = potential_captures[i] & penultimate_rank_bitboard;
            potential_captures[i] &= ~penultimate_rank_bitboard;
            while (potential_captures[i]) {
                const auto pawn_idx = pop_min_bit(potential_captures[i]);
                move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + shifts[i], pawn_idx));
            }
            while (promotion_captures[i]) {
                const auto pawn_idx = pop_min_bit(promotion_captures[i]);
                move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
                move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
                move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
                move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
            }
        }
    }

    Bitboard potential_ep = pawn_mask & ep_rank_bitboard;
    while (potential_ep) {
        const auto pawn_idx = pop_min_bit(potential_ep);
        if (std::abs(board.get_en_passant_file() - static_cast<int>(get_file(pawn_idx))) == 1) {
            const auto ep_target_square = get_position(((int) get_rank(pawn_idx)), board.get_en_passant_file()) + ahead_offset;
            const Bitboard cleared_bitboard = total_occupancy ^ idx_to_bitboard(pawn_idx) ^ idx_to_bitboard(ep_target_square) ^
                                                idx_to_bitboard(ep_target_square - ahead_offset);
            const Bitboard threatening_bishops =
                generate_bishop_movemask(cleared_bitboard, king_idx) & (board.get_bishop_occupancy(enemy_side) | board.get_queen_occupancy(enemy_side));
            const Bitboard threatening_rooks =
                generate_rook_movemask(cleared_bitboard, king_idx) & (board.get_rook_occupancy(enemy_side) | board.get_queen_occupancy(enemy_side));
            // We only need to worry about discovered checks from sliders; you cannot end a turn in check 

            if (threatening_bishops == 0 && threatening_rooks == 0) {
                move_list.add_move(Move(MoveFlags::EN_PASSANT_CAPTURE, ep_target_square, pawn_idx));
            }
        }
    }

    // For pinned pieces
    {
        Bitboard advancing_board = (side == Side::WHITE ? pinned_pawns << 8 : pinned_pawns >> 8) & ~total_occupancy & valid_in_check_moves & (file_bitboard << get_file(king_idx));
        Bitboard double_advancing_board = (side == Side::WHITE ? advancing_board << 8 : advancing_board >> 8) & ~total_occupancy & valid_in_check_moves;
        advancing_board = side == Side::WHITE ? advancing_board >> 8 : advancing_board << 8;
        double_advancing_board = side == Side::WHITE ? double_advancing_board >> 16 : double_advancing_board << 16;
        double_advancing_board &= start_rank_bitboard;

        // Only one piece can be pinned in a given direction
        if (advancing_board != 0) {
            const auto pawn_idx = get_lsb(advancing_board);
            move_list.add_move(Move(MoveFlags::QUIET_MOVE, pawn_idx + ahead_offset, pawn_idx));
            // If it can double advance it must be the same as the single advancing piece
            if (double_advancing_board != 0) {
                move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, pawn_idx + (2 * ahead_offset), pawn_idx));
            }
        }
        // If a pawn is pinned, it cannot be quietly promoted

        Bitboard potential_captures[2];
        int shifts[2];
        potential_captures[0] = (side == Side::WHITE ? pinned_pawns << 9 : pinned_pawns >> 7) & enemy_occupancy & ~a_file & valid_in_check_moves;
        potential_captures[0] = side == Side::WHITE ? potential_captures[0] >> 9 : potential_captures[0] << 7;
        potential_captures[1] = (side == Side::WHITE ? pinned_pawns << 7 : pinned_pawns >> 9) & enemy_occupancy & ~h_file & valid_in_check_moves;
        potential_captures[1] = side == Side::WHITE ? potential_captures[1] >> 7 : potential_captures[1] << 9;

        shifts[0] = side == Side::WHITE ? 9 : -7;
        shifts[1] = side == Side::WHITE ? 7 : -9;

        for (int i = 0; i < 2; i++) {
            potential_captures[i] &= MagicNumbers::DiagonalSquares[(2 * king_idx) + (i ^ static_cast<int>(side))];
            const auto pawn_idx = get_lsb(potential_captures[i]);
            if ((potential_captures[i] & penultimate_rank_bitboard) != 0) {
                move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
                move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
                move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
                move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + shifts[i], pawn_idx));
            } else if ((potential_captures[i] & ~penultimate_rank_bitboard) != 0) {
                move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + shifts[i], pawn_idx));
            }
        }
    }
}