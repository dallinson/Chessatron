#pragma once

#include <cstdint>

#include "chessboard.hpp"
#include "move.hpp"
#include "utils.hpp"

enum class MoveGenType {
    ALL_LEGAL,
    QUIESCENCE,
    NON_QUIESCENCE,
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

    if (gen_type != MoveGenType::QUIESCENCE && checking_piece_count == 0) {
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
    return MagicNumbers::KnightMoves[idx];
}

template <>
inline Bitboard MoveGenerator::generate_movemask<PieceTypes::KING>(const Bitboard b, const int idx) {
    (void) b;
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
        if constexpr (gen_type == MoveGenType::QUIESCENCE) {
            potential_moves &= enemy_occupancy;
        } else if constexpr (gen_type == MoveGenType::NON_QUIESCENCE) {
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

template <MoveGenType gen_type> void MoveGenerator::generate_pawn_moves(const ChessBoard& c, const Side side, MoveList& move_list) {
    Bitboard pawn_mask = c.get_pawn_occupancy(side);
    const Side enemy_side = ENEMY_SIDE(side);
    const Bitboard enemy_occupancy = c.get_occupancy(enemy_side);
    const Bitboard total_occupancy = enemy_occupancy | c.get_occupancy(side);
    const auto king_idx = get_lsb(c.get_king_occupancy(side));
    const auto ahead_offset = side == Side::WHITE ? 8 : -8;
    const auto capture_front_left = side == Side::WHITE ? 7 : -7;
    const auto capture_front_right = side == Side::WHITE ? 9 : -9;
    const auto left_wall = side == Side::WHITE ? 0u : 7u;
    const auto right_wall = side == Side::WHITE ? 7u : 0u;

    const auto checking_idx = get_lsb(c.get_checkers(side));

    const auto start_rank = side == Side::WHITE ? 1u : 6u;
    const auto penultimate_rank = side == Side::WHITE ? 6u : 1u;
    const auto ep_rank = side == Side::WHITE ? 4u : 3u;
    while (pawn_mask) {
        int pawn_idx = pop_min_bit(pawn_mask);
        if (get_bit(total_occupancy, pawn_idx + ahead_offset) == 0 &&
            (get_bit(c.get_pinned_pieces(side), pawn_idx) == 0 || get_file(pawn_idx) == get_file(king_idx))) {
            // if not pinned, or the pawn motion is aligned with the king
            if constexpr (gen_type != MoveGenType::QUIESCENCE) {
                if ((get_rank(pawn_idx) == start_rank && get_bit(total_occupancy, pawn_idx + ahead_offset + ahead_offset) == 0) &&
                    (checking_idx == 64 || (idx_to_bitboard(pawn_idx + ahead_offset + ahead_offset) &
                                            MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx]) != 0)) {
                    move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, pawn_idx + ahead_offset + ahead_offset, pawn_idx));
                }
            }

            if (checking_idx == 64 ||
                (idx_to_bitboard(pawn_idx + ahead_offset) & MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx]) != 0) {
                // if not in check or can block

                if (get_rank(pawn_idx) == penultimate_rank) {
                    if constexpr (gen_type != MoveGenType::NON_QUIESCENCE) move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                    if constexpr (gen_type != MoveGenType::NON_QUIESCENCE) move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                    if constexpr (gen_type != MoveGenType::QUIESCENCE) move_list.add_move(Move(MoveFlags::ROOK_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                    if constexpr (gen_type != MoveGenType::QUIESCENCE) move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION, pawn_idx + ahead_offset, pawn_idx));
                } else {
                    if constexpr (gen_type != MoveGenType::QUIESCENCE) move_list.add_move(Move(MoveFlags::QUIET_MOVE, pawn_idx + ahead_offset, pawn_idx));
                }
            }
        }
        // This handles advancing
        
        if constexpr (gen_type != MoveGenType::NON_QUIESCENCE) {
            if ((get_file(pawn_idx) != left_wall && get_bit(enemy_occupancy, pawn_idx + capture_front_left)) &&
                // if there is a piece we _can_ capture
                (get_bit(c.get_pinned_pieces(side), pawn_idx) == 0 || is_aligned(king_idx, pawn_idx, pawn_idx + capture_front_left)) &&
                // and we're not pinned/are moving in the capture direction
                (checking_idx == 64 || checking_idx == pawn_idx + capture_front_left)) {
                // or we want are capturing the checking piece
                if (get_rank(pawn_idx) != penultimate_rank) {
                    move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + capture_front_left, pawn_idx));
                }
            }

            if ((get_file(pawn_idx) != right_wall && get_bit(enemy_occupancy, pawn_idx + capture_front_right)) &&
                // if there is a piece we _can_ capture
                (get_bit(c.get_pinned_pieces(side), pawn_idx) == 0 || is_aligned(king_idx, pawn_idx, pawn_idx + capture_front_right)) &&
                // and we're not pinned/are moving in the capture direction
                (checking_idx == 64 || checking_idx == pawn_idx + capture_front_right)) {
                // or we want are capturing the checking piece
                if (get_rank(pawn_idx) != penultimate_rank) {
                    move_list.add_move(Move(MoveFlags::CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                } else {
                    move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                    move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                    move_list.add_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                    move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, pawn_idx + capture_front_right, pawn_idx));
                }
            }

            if (get_rank(pawn_idx) == ep_rank && std::abs(c.get_en_passant_file() - static_cast<int>(get_file(pawn_idx))) == 1) {
                // if a pseudolegal en passant is possible
                const auto ep_target_square = get_position(((int) get_rank(pawn_idx)) + (ahead_offset / 8), c.get_en_passant_file());
                const Bitboard cleared_bitboard = total_occupancy ^ idx_to_bitboard(pawn_idx) ^ idx_to_bitboard(ep_target_square) ^
                                                  idx_to_bitboard(ep_target_square - ahead_offset);
                const Bitboard threatening_bishops =
                    generate_bishop_movemask(cleared_bitboard, king_idx) & (c.get_bishop_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));
                const Bitboard threatening_rooks =
                    generate_rook_movemask(cleared_bitboard, king_idx) & (c.get_rook_occupancy(enemy_side) | c.get_queen_occupancy(enemy_side));

                if (threatening_bishops == 0 && threatening_rooks == 0) {
                    move_list.add_move(Move(MoveFlags::EN_PASSANT_CAPTURE, ep_target_square, pawn_idx));
                }
            }
        }
    }
}