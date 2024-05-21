#pragma once

#include <cstdint>
#include <iostream>


#include "chessboard.hpp"
#include "move.hpp"
#include "utils.hpp"

enum class MoveGenType {
    ALL_LEGAL,
    QUIESCENCE,
    NON_QUIESCENCE,
    GOOD_PROMOS,
    BAD_PROMOS,
    CAPTURES,
    QUIETS,
};

using enum MoveGenType;

constexpr bool gen_captures(MoveGenType gen_type) { return gen_type == ALL_LEGAL || gen_type == QUIESCENCE || gen_type == CAPTURES; };
constexpr bool gen_good_promos(MoveGenType gen_type) { return gen_type == ALL_LEGAL || gen_type == QUIESCENCE || gen_type == GOOD_PROMOS; };
constexpr bool gen_bad_promos(MoveGenType gen_type) { return gen_type == ALL_LEGAL || gen_type == NON_QUIESCENCE || gen_type == BAD_PROMOS; };
constexpr bool gen_quiets(MoveGenType gen_type) { return gen_type == ALL_LEGAL || gen_type == NON_QUIESCENCE || gen_type == QUIETS; };
constexpr bool gen_non_pawn(MoveGenType gen_type) { return gen_type != GOOD_PROMOS && gen_type != BAD_PROMOS; };
constexpr bool gen_promos(MoveGenType gen_type) { return gen_good_promos(gen_type) || gen_bad_promos(gen_type); };
constexpr bool gen_only_promos(MoveGenType gen_type) { return gen_type == GOOD_PROMOS || gen_type == BAD_PROMOS; };
constexpr bool gen_only_captures(MoveGenType gen_type) { return gen_type == QUIESCENCE || gen_type == CAPTURES; };
constexpr bool gen_only_quiets(MoveGenType gen_type) { return gen_type == NON_QUIESCENCE || gen_type == QUIETS; };

namespace MoveGenerator {
    template <MoveGenType gen_type> MoveList generate_legal_moves(const ChessBoard& c, const Side side);
    int get_checking_piece_count(const ChessBoard& c, const Side side);
    Bitboard get_checkers(const ChessBoard& c, const Side side);
    Bitboard get_attackers(const ChessBoard& board, const Side side, const int target_idx, const Bitboard occupancy);

    Bitboard generate_bishop_mm(const Bitboard b, const int idx);
    Bitboard generate_rook_mm(const Bitboard b, const int idx);
    Bitboard generate_queen_mm(const Bitboard b, const int idx);
    template <PieceTypes piece_type> Bitboard generate_mm(const Bitboard b, const int idx);

    template <PieceTypes piece_type, MoveGenType gen_type> void generate_moves(const ChessBoard& c, const Side side, MoveList& move_list);
    template <MoveGenType gen_type, Side stm> void generate_pawn_moves(const ChessBoard& c, MoveList& move_list);
    void generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list);

    bool is_move_legal(const ChessBoard& c, const Move m);
    bool is_move_pseudolegal(const ChessBoard& c, const Move to_test);

    template <MoveGenType gen_type> MoveList generate_legal_moves(const ChessBoard& c, const Side side);
} // namespace MoveGenerator

template <MoveGenType gen_type> MoveList MoveGenerator::generate_legal_moves(const ChessBoard& c, const Side side) {
    MoveList to_return;

    if constexpr (gen_non_pawn(gen_type)) MoveGenerator::generate_moves<PieceTypes::KING, gen_type>(c, side, to_return);

    int checking_piece_count = std::popcount(c.checkers());

    if (checking_piece_count >= 2) {
        return to_return;
    }

    if (gen_quiets(gen_type) && gen_non_pawn(gen_type) && checking_piece_count == 0) {
        MoveGenerator::generate_castling_moves(c, side, to_return);
    }
    if constexpr (gen_non_pawn(gen_type)) MoveGenerator::generate_moves<PieceTypes::QUEEN, gen_type>(c, side, to_return);
    if constexpr (gen_non_pawn(gen_type)) MoveGenerator::generate_moves<PieceTypes::BISHOP, gen_type>(c, side, to_return);
    if constexpr (gen_non_pawn(gen_type)) MoveGenerator::generate_moves<PieceTypes::KNIGHT, gen_type>(c, side, to_return);
    if constexpr (gen_non_pawn(gen_type)) MoveGenerator::generate_moves<PieceTypes::ROOK, gen_type>(c, side, to_return);
    if (side == Side::WHITE) {
        MoveGenerator::generate_pawn_moves<gen_type, Side::WHITE>(c, to_return);
    } else {
        MoveGenerator::generate_pawn_moves<gen_type, Side::BLACK>(c, to_return);
    }

    return to_return;
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::BISHOP>(const Bitboard b, const int idx) {
    return generate_bishop_mm(b, idx);
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::ROOK>(const Bitboard b, const int idx) {
    return generate_rook_mm(b, idx);
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::QUEEN>(const Bitboard b, const int idx) {
    return generate_queen_mm(b, idx);
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::KNIGHT>(const Bitboard b, const int idx) {
    (void) b;
    return MagicNumbers::KnightMoves[idx];
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::KING>(const Bitboard b, const int idx) {
    (void) b;
    return MagicNumbers::KingMoves[idx];
}

template <PieceTypes piece_type, MoveGenType gen_type> void MoveGenerator::generate_moves(const ChessBoard& c, const Side stm, MoveList& output) {
    if constexpr (piece_type == PieceTypes::PAWN) {
        return generate_pawn_moves<gen_type>(c, stm, output);
    }
    const int king_idx = get_lsb(c.kings(stm));
    const Bitboard friendly_bb = c.occupancy(stm);
    const Bitboard enemy_bb = c.occupancy(enemy_side(stm));
    const Bitboard all_bb = enemy_bb | friendly_bb;
    const auto checking_idx = get_lsb(c.checkers());
    const auto enemy = enemy_side(stm);
    Bitboard pieces = c.pieces<piece_type>(stm);
    while (pieces) {
        const auto piece_idx = pop_lsb(pieces);
        auto potential_moves = generate_mm<piece_type>(all_bb, piece_idx) & ~friendly_bb;
        if constexpr (gen_only_captures(gen_type)) {
            potential_moves &= enemy_bb;
        } else if constexpr (gen_only_quiets(gen_type)) {
            potential_moves &= ~enemy_bb;
        }
        if constexpr (piece_type != PieceTypes::KING) {
            if (checking_idx != 64) {
                // no checking pieces implies the checking idx is 64
                potential_moves &= MagicNumbers::ConnectingSquares[(64 * king_idx) + checking_idx];
            }
            if ((idx_to_bb(piece_idx) & c.pinned_pieces()) != 0) {
                // if we are pinned
                potential_moves &= MagicNumbers::AlignedSquares[(64 * king_idx) + piece_idx];
            }
            // these are the only valid move positions
        }

        while (potential_moves) {
            const auto target_idx = pop_lsb(potential_moves);
            if constexpr (piece_type == PieceTypes::KING) {
                const Bitboard cleared_bb = all_bb ^ idx_to_bb(king_idx);
                if (get_attackers(c, enemy, target_idx, cleared_bb)) {
                    continue;
                }
            }
            const auto flag = (get_bit(c.occupancy(), target_idx) != 0)
                ? MoveFlags::CAPTURE
                : MoveFlags::QUIET_MOVE;
            output.add_move(Move(flag, target_idx, piece_idx));
        }
    }
}

template <MoveGenType gen_type, MoveFlags base_flags> void gen_promotions(MoveList& move_list, const uint8_t src, const uint8_t dst) {
    if constexpr (gen_good_promos(gen_type)) move_list.add_move(Move(MoveFlags::QUEEN_PROMOTION | base_flags, dst, src));
    if constexpr (gen_good_promos(gen_type)) move_list.add_move(Move(MoveFlags::KNIGHT_PROMOTION | base_flags, dst, src));
    if constexpr (gen_bad_promos(gen_type)) move_list.add_move(Move(MoveFlags::ROOK_PROMOTION | base_flags, dst, src));
    if constexpr (gen_bad_promos(gen_type)) move_list.add_move(Move(MoveFlags::BISHOP_PROMOTION | base_flags, dst, src));
}

template <MoveGenType gen_type, Side stm> void MoveGenerator::generate_pawn_moves(const ChessBoard& c, MoveList& move_list) {
    const auto friendly_pawns = c.pawns(stm);
    const auto occupied = c.occupancy();
    const auto pinned_pawns = friendly_pawns & c.pinned_pieces();
    const auto unpinned_pawns = pinned_pawns ^ friendly_pawns;
    const auto enemy_bb = c.occupancy(enemy_side(stm));
    const auto ksq = get_lsb(c.kings(stm));
    constexpr auto ahead = stm == Side::WHITE ? 8 : -8;
    constexpr auto back_rank = stm == Side::WHITE ? 7 : 0;
    constexpr auto back_rank_bb = rank_bb(back_rank);
    const auto valid_dests = c.in_check() ? MagicNumbers::ConnectingSquares[(64 * ksq) + get_lsb(c.checkers())] : 0xFFFFFFFFFFFFFFFF;

    const auto advanceable = unpinned_pawns | (pinned_pawns & file_bb(file(ksq)));
    auto advancing = (stm == Side::WHITE ? advanceable << 8 : advanceable >> 8) & ~occupied;
    if constexpr (gen_quiets(gen_type)) {
        auto double_advancing = (stm == Side::WHITE ? advancing << 8 : advancing >> 8) & ~occupied & valid_dests;
        double_advancing &= rank_bb(stm == Side::WHITE ? 3 : 4);

        while (double_advancing) {
            const auto lsb = pop_lsb(double_advancing);
            move_list.add_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, lsb, lsb - (2 * ahead)));
        }
    }

    // promotions can be generated both within and outside 
    advancing &= valid_dests;
    auto promotable = advancing & rank_bb(stm == Side::WHITE ? 7 : 0);
    auto non_promotable = promotable ^ advancing;
    while (promotable) {
        const auto lsb = pop_lsb(promotable);
        gen_promotions<gen_type, MoveFlags::QUIET_MOVE>(move_list, lsb - ahead, lsb);
    }

    if constexpr (gen_quiets(gen_type)) {
        while (non_promotable) {
            const auto lsb = pop_lsb(non_promotable);
            move_list.add_move(Move(MoveFlags::QUIET_MOVE, lsb, lsb - ahead));
        }
    }

    if constexpr (gen_promos(gen_type) || gen_captures(gen_type)) {
        // now gen captures
        {
            // towards a file
            constexpr auto a_file = file_bb(0);
            constexpr auto offset = stm == Side::WHITE ? 7 : -9;
            Bitboard capturing_pieces = ([&]() {
                const auto invalid_bb = a_file | back_rank_bb;
                if (c.kings(stm) & invalid_bb) return static_cast<Bitboard>(0);
                return pinned_pawns & MagicNumbers::AlignedSquares[(64 * ksq) + (ksq + offset)];
            }() | unpinned_pawns) & ~a_file;
            capturing_pieces = (stm == Side::WHITE ? capturing_pieces << 7 : capturing_pieces >> 9) & enemy_bb & valid_dests;
            while (capturing_pieces) {
                const auto lsb = pop_lsb(capturing_pieces);
                if (rank(lsb) == back_rank) {
                    gen_promotions<MoveGenType::ALL_LEGAL, MoveFlags::CAPTURE>(move_list, lsb - offset, lsb);
                } else if (!gen_only_promos(gen_type)) {
                    move_list.add_move(Move(MoveFlags::CAPTURE, lsb, lsb - offset));
                }
            }
        }
        {
            // towards h file
            constexpr auto h_file = file_bb(7);
            constexpr auto offset = stm == Side::WHITE ? 9 : -7;
            Bitboard capturing_pieces = ([&]() {
                const auto invalid_bb = h_file | back_rank_bb;
                if (c.kings(stm) & invalid_bb) return static_cast<Bitboard>(0);
                return pinned_pawns & MagicNumbers::AlignedSquares[(64 * ksq) + (ksq + offset)];
            }() | unpinned_pawns) & ~h_file;
            capturing_pieces = (stm == Side::WHITE ? capturing_pieces << 9 : capturing_pieces >> 7) & enemy_bb & valid_dests;
            while (capturing_pieces) {
                const auto lsb = pop_lsb(capturing_pieces);
                if (rank(lsb) == back_rank) {
                    gen_promotions<MoveGenType::ALL_LEGAL, MoveFlags::CAPTURE>(move_list, lsb - offset, lsb);
                } else if (!gen_only_promos(gen_type)) {
                    move_list.add_move(Move(MoveFlags::CAPTURE, lsb, lsb - offset));
                }
            }
        }

        if constexpr (gen_captures(gen_type)) {
            constexpr std::array<Bitboard, 10> ep_masks { 0x202020202020202, 0x505050505050505, 0xa0a0a0a0a0a0a0a, 0x1414141414141414, 0x2828282828282828, 0x5050505050505050, 0xa0a0a0a0a0a0a0a0, 0x4040404040404040, 0, 0 };
            constexpr Bitboard ep_rank_mask = stm == Side::WHITE ? rank_bb(4) : rank_bb(3);
            auto ep_pawns = ep_masks[c.get_en_passant_file()] & ep_rank_mask & c.pawns(stm);
            while (ep_pawns) {
                const auto lsb = pop_lsb(ep_pawns);
                constexpr auto ep_offset = stm == Side::WHITE ? 1 : -1;
                const auto ep_target_square = static_cast<int>(get_position((stm == Side::WHITE ? 4 : 3) + ep_offset, c.get_en_passant_file()));
                const auto cleared_bb = occupied ^ idx_to_bb(lsb) ^ idx_to_bb(ep_target_square) ^ idx_to_bb(ep_target_square - (8 * ep_offset));
                const Bitboard threatening_bishops =
                    generate_bishop_mm(cleared_bb, ksq) & (c.bishops(enemy_side(stm)) | c.queens(enemy_side(stm)));
                const Bitboard threatening_rooks =
                    generate_rook_mm(cleared_bb, ksq) & (c.rooks(enemy_side(stm)) | c.queens(enemy_side(stm)));

                if (threatening_bishops == 0 && threatening_rooks == 0) {
                    move_list.add_move(Move(MoveFlags::EN_PASSANT_CAPTURE, ep_target_square, lsb));
                }
            }
        }
    }
}