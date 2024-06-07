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
};

namespace MoveGenerator {
    template <MoveGenType gen_type> MoveList generate_legal_moves(const Position& c, const Side side);
    int get_checking_piece_count(const Position& c, const Side side);
    Bitboard get_checkers(const Position& c, const Side side);
    Bitboard get_attackers(const Position& board, const Side side, const Square target_sq, const Bitboard occupancy);

    Bitboard generate_bishop_mm(const Bitboard b, const Square sq);
    Bitboard generate_rook_mm(const Bitboard b, const Square sq);
    Bitboard generate_queen_mm(const Bitboard b, const Square sq);
    template <PieceTypes piece_type> Bitboard generate_mm(const Bitboard b, const Square sq);

    template <PieceTypes piece_type, MoveGenType gen_type> void generate_moves(const Position& c, const Side side, MoveList& move_list);
    template <MoveGenType gen_type, Side stm> void generate_pawn_moves(const Position& c, MoveList& move_list);
    void generate_castling_moves(const Position& c, const Side side, MoveList& move_list);

    bool is_move_legal(const Position& c, const Move m);
    bool is_move_pseudolegal(const Position& c, const Move to_test);

    template <MoveGenType gen_type> MoveList generate_legal_moves(const Position& c, const Side side);
} // namespace MoveGenerator

template <MoveGenType gen_type> MoveList MoveGenerator::generate_legal_moves(const Position& c, const Side side) {
    MoveList to_return;

    MoveGenerator::generate_moves<PieceTypes::KING, gen_type>(c, side, to_return);

    int checking_piece_count = c.checkers().popcnt();

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
    if (side == Side::WHITE) {
        MoveGenerator::generate_pawn_moves<gen_type, Side::WHITE>(c, to_return);
    } else {
        MoveGenerator::generate_pawn_moves<gen_type, Side::BLACK>(c, to_return);
    }

    return to_return;
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::BISHOP>(const Bitboard b, const Square sq) {
    return generate_bishop_mm(b, sq);
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::ROOK>(const Bitboard b, const Square sq) {
    return generate_rook_mm(b, sq);
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::QUEEN>(const Bitboard b, const Square sq) {
    return generate_queen_mm(b, sq);
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::KNIGHT>(const Bitboard b, const Square sq) {
    (void) b;
    return MagicNumbers::KnightMoves[sq_to_int(sq)];
}

template <>
inline Bitboard MoveGenerator::generate_mm<PieceTypes::KING>(const Bitboard b, const Square sq) {
    (void) b;
    return MagicNumbers::KingMoves[sq_to_int(sq)];
}

template <PieceTypes piece_type, MoveGenType gen_type> void MoveGenerator::generate_moves(const Position& c, const Side stm, MoveList& output) {
    if constexpr (piece_type == PieceTypes::PAWN) {
        return generate_pawn_moves<gen_type>(c, stm, output);
    }
    const auto king_idx = c.kings(stm).lsb();
    const Bitboard friendly_bb = c.occupancy(stm);
    const Bitboard enemy_bb = c.occupancy(enemy_side(stm));
    const Bitboard all_bb = enemy_bb | friendly_bb;
    const auto enemy = enemy_side(stm);
    Bitboard pieces = c.pieces<piece_type>(stm);
    while (!pieces.empty()) {
        const auto piece_idx = pieces.pop_lsb();
        auto potential_moves = generate_mm<piece_type>(all_bb, piece_idx) & ~friendly_bb;
        if constexpr (gen_type == MoveGenType::QUIESCENCE) {
            potential_moves &= enemy_bb;
        } else if constexpr (gen_type == MoveGenType::NON_QUIESCENCE) {
            potential_moves &= ~enemy_bb;
        }
        if constexpr (piece_type != PieceTypes::KING) {
            if (!c.checkers().empty()) {
                // no checking pieces implies the checking idx is 64
                potential_moves &= MagicNumbers::ConnectingSquares[(64 * sq_to_int(king_idx)) + sq_to_int(c.checkers().lsb())];
            }
            if (!(c.pinned_pieces() & piece_idx).empty()) {
                // if we are pinned
                potential_moves &= MagicNumbers::AlignedSquares[(64 * sq_to_int(king_idx)) + sq_to_int(piece_idx)];
            }
            // these are the only valid move positions
        }

        while (!potential_moves.empty()) {
            const auto target_idx = potential_moves.pop_lsb();
            if constexpr (piece_type == PieceTypes::KING) {
                const Bitboard cleared_bb = all_bb ^ king_idx;
                if (!get_attackers(c, enemy, target_idx, cleared_bb).empty()) {
                    continue;
                }
            }
            const auto flag = (c.occupancy()[target_idx])
                ? MoveFlags::CAPTURE
                : MoveFlags::QUIET_MOVE;
            output.add(Move(flag, target_idx, piece_idx));
        }
    }
}

template <MoveGenType gen_type, MoveFlags base_flags> void gen_promotions(MoveList& move_list, const Square src, const Square dst) {
    if constexpr (gen_type != MoveGenType::NON_QUIESCENCE) move_list.add(Move(MoveFlags::QUEEN_PROMOTION | base_flags, dst, src));
    if constexpr (gen_type != MoveGenType::NON_QUIESCENCE) move_list.add(Move(MoveFlags::KNIGHT_PROMOTION | base_flags, dst, src));
    if constexpr (gen_type != MoveGenType::QUIESCENCE) move_list.add(Move(MoveFlags::ROOK_PROMOTION | base_flags, dst, src));
    if constexpr (gen_type != MoveGenType::QUIESCENCE) move_list.add(Move(MoveFlags::BISHOP_PROMOTION | base_flags, dst, src));
}

template <MoveGenType gen_type, Side stm> void MoveGenerator::generate_pawn_moves(const Position& c, MoveList& move_list) {
    const auto friendly_pawns = c.pawns(stm);
    const auto occupied = c.occupancy();
    const auto pinned_pawns = friendly_pawns & c.pinned_pieces();
    const auto unpinned_pawns = pinned_pawns ^ friendly_pawns;
    const auto enemy_bb = c.occupancy(enemy_side(stm));
    const auto ksq = c.kings(stm).lsb();
    constexpr auto ahead = stm == Side::WHITE ? 8 : -8;
    constexpr auto back_rank = stm == Side::WHITE ? 7 : 0;
    constexpr auto back_rank_bb = rank_bb(back_rank);
    const auto valid_dests = c.in_check() ? MagicNumbers::ConnectingSquares[(64 * sq_to_int(ksq)) + sq_to_int(c.checkers().lsb())] : Bitboard(0xFFFFFFFFFFFFFFFF);

    const auto advanceable = unpinned_pawns | (pinned_pawns & file_bb(file(ksq)));
    auto advancing = (stm == Side::WHITE ? advanceable << 8 : advanceable >> 8) & ~occupied;
    if constexpr (gen_type != MoveGenType::QUIESCENCE) {
        auto double_advancing = (stm == Side::WHITE ? advancing << 8 : advancing >> 8) & ~occupied & valid_dests;
        double_advancing &= rank_bb(stm == Side::WHITE ? 3 : 4);

        while (!double_advancing.empty()) {
            const auto lsb = double_advancing.pop_lsb();
            move_list.add(Move(MoveFlags::DOUBLE_PAWN_PUSH, lsb, lsb - (2 * ahead)));
        }
    }

    // promotions can be generated both within and outside 
    advancing &= valid_dests;
    auto promotable = advancing & rank_bb(stm == Side::WHITE ? 7 : 0);
    auto non_promotable = promotable ^ advancing;
    while (!promotable.empty()) {
        const auto lsb = promotable.pop_lsb();
        gen_promotions<gen_type, MoveFlags::QUIET_MOVE>(move_list, lsb - ahead, lsb);
    }

    if constexpr (gen_type != MoveGenType::QUIESCENCE) {
        while (!non_promotable.empty()) {
            const auto lsb = non_promotable.pop_lsb();
            move_list.add(Move(MoveFlags::QUIET_MOVE, lsb, lsb - ahead));
        }
    }

    if constexpr (gen_type != MoveGenType::NON_QUIESCENCE) {
        // now gen captures
        {
            // towards a file
            constexpr auto a_file = file_bb(0);
            constexpr auto offset = stm == Side::WHITE ? 7 : -9;
            Bitboard capturing_pieces = ([&]() {
                const auto invalid_bb = a_file | back_rank_bb;
                if (!(c.kings(stm) & invalid_bb).empty()) return static_cast<Bitboard>(0);
                return pinned_pawns & MagicNumbers::AlignedSquares[(64 * sq_to_int(ksq)) + sq_to_int(ksq + offset)];
            }() | unpinned_pawns) & ~a_file;
            capturing_pieces = (stm == Side::WHITE ? capturing_pieces << 7 : capturing_pieces >> 9) & enemy_bb & valid_dests;
            while (!capturing_pieces.empty()) {
                const auto lsb = capturing_pieces.pop_lsb();
                if (rank(lsb) == back_rank) {
                    gen_promotions<MoveGenType::ALL_LEGAL, MoveFlags::CAPTURE>(move_list, lsb - offset, lsb);
                } else {
                    move_list.add(Move(MoveFlags::CAPTURE, lsb, lsb - offset));
                }
            }
        }
        {
            // towards h file
            constexpr auto h_file = file_bb(7);
            constexpr auto offset = stm == Side::WHITE ? 9 : -7;
            Bitboard capturing_pieces = ([&]() {
                const auto invalid_bb = h_file | back_rank_bb;
                if (!(c.kings(stm) & invalid_bb).empty()) return static_cast<Bitboard>(0);
                return pinned_pawns & MagicNumbers::AlignedSquares[(64 * sq_to_int(ksq)) + sq_to_int(ksq + offset)];
            }() | unpinned_pawns) & ~h_file;
            capturing_pieces = (stm == Side::WHITE ? capturing_pieces << 9 : capturing_pieces >> 7) & enemy_bb & valid_dests;
            while (!capturing_pieces.empty()) {
                const auto lsb = capturing_pieces.pop_lsb();
                if (rank(lsb) == back_rank) {
                    gen_promotions<MoveGenType::ALL_LEGAL, MoveFlags::CAPTURE>(move_list, lsb - offset, lsb);
                } else {
                    move_list.add(Move(MoveFlags::CAPTURE, lsb, lsb - offset));
                }
            }
        }


        constexpr std::array<Bitboard, 10> ep_masks { 0x202020202020202, 0x505050505050505, 0xa0a0a0a0a0a0a0a, 0x1414141414141414, 0x2828282828282828, 0x5050505050505050, 0xa0a0a0a0a0a0a0a0, 0x4040404040404040, 0, 0 };
        constexpr Bitboard ep_rank_mask = stm == Side::WHITE ? rank_bb(4) : rank_bb(3);
        auto ep_pawns = ep_masks[c.get_en_passant_file()] & ep_rank_mask & c.pawns(stm);
        while (!ep_pawns.empty()) {
            const auto lsb = ep_pawns.pop_lsb();
            constexpr auto ep_offset = stm == Side::WHITE ? 1 : -1;
            const auto ep_target_square = get_position((stm == Side::WHITE ? 4 : 3) + ep_offset, c.get_en_passant_file());
            const auto cleared_bb = occupied ^ lsb ^ ep_target_square ^ (ep_target_square - (8 * ep_offset));
            const Bitboard threatening_bishops =
                generate_bishop_mm(cleared_bb, ksq) & (c.bishops(enemy_side(stm)) | c.queens(enemy_side(stm)));
            const Bitboard threatening_rooks =
                generate_rook_mm(cleared_bb, ksq) & (c.rooks(enemy_side(stm)) | c.queens(enemy_side(stm)));

            if (threatening_bishops.empty() && threatening_rooks.empty()) {
                move_list.add(Move(MoveFlags::EN_PASSANT_CAPTURE, ep_target_square, lsb));
            }
        }
    }
}