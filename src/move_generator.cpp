#include "move_generator.hpp"

#include <immintrin.h>

#include "magic_numbers.hpp"

MoveList MoveGenerator::generate_moves(ChessBoard& c, int side) {
    MoveList to_return = MoveGenerator::generate_king_moves(c, side);

    if (is_in_double_check(c, side, bitboard_to_idx(c.get_king_occupancy(side)))) {
        return to_return;
    }

    to_return.add_moves(MoveGenerator::generate_queen_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_bishop_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_knight_moves(c, side));
    to_return.add_moves(MoveGenerator::generate_rook_moves(c, side));



    return to_return;
}

bool MoveGenerator::is_in_double_check(ChessBoard& c, int side, int king_idx) {
    uint64_t bishop_mask = MoveGenerator::generate_bishop_movemask(c, king_idx);
    uint64_t rook_mask = MoveGenerator::generate_rook_movemask(c, king_idx);

    int enemy_side = (side + 1) & 0x01;
    uint64_t enemy_queen_mask = c.get_queen_occupancy(enemy_side);

    int checking_pieces = _mm_popcnt_u64(enemy_queen_mask & (bishop_mask | rook_mask));
    checking_pieces += _mm_popcnt_u64(c.get_bishop_occupancy(enemy_side) & bishop_mask);
    checking_pieces += _mm_popcnt_u64(c.get_rook_occupancy(enemy_side) & rook_mask);
    // this calculates the checking pieces for the sliding pieces
    checking_pieces += _mm_popcnt_u64(c.get_knight_occupancy(enemy_side) & knightMoves[king_idx]);

    uint64_t pawns_attacking_here = pawnAttackMaps[(64 * side) + king_idx];
    // black is in the latter 64 spaces
    checking_pieces += _mm_popcnt_u64(c.get_pawn_occupancy(enemy_side) & pawns_attacking_here);

    return checking_pieces >= 2;
}

MoveList MoveGenerator::generate_king_moves(ChessBoard& c, int side) {
    int king_idx = bitboard_to_idx(c.get_king_occupancy(side));
    uint64_t king_moves = kingMoves[king_idx];
    return filter_to_valid_moves(c, side, king_moves, king_idx);
}

MoveList MoveGenerator::filter_to_valid_moves(ChessBoard& c, int side, uint64_t potential_moves, int idx) {
    MoveList to_return;
    uint64_t valid_moves = potential_moves & ~c.get_side_occupancy(side);
    // only move onto spaces unoccupied by friendlies
    int src_idx = idx & 0x3F;
    while (potential_moves) {
        int move_idx = pop_min_bit(&potential_moves);
        uint_fast16_t flags = 0;
        flags |= ((GET_BIT(c.get_side_occupancy((side + 1) & 1), move_idx)) << 2);

        to_return.add_move((Move) (((flags & 0x0F) << 12) | ((move_idx & 0x3F) << 6) | src_idx));
    }
    return to_return;
}

uint64_t MoveGenerator::generate_bishop_movemask(ChessBoard& c, int idx) {
    int masked = (c.get_occupancy() & BMask[idx]);
    return BAttacks[(masked * BMagic[idx]) >> BBits[idx]];
}

uint64_t MoveGenerator::generate_rook_movemask(ChessBoard& c, int idx) {
    int masked = (c.get_occupancy() & RMask[idx]);
    return RAttacks[(masked * RMagic[idx]) >> RBits[idx]];
}

uint64_t MoveGenerator::generate_queen_movemask(ChessBoard& c, int idx) {
    return MoveGenerator::generate_bishop_movemask(c, idx) | MoveGenerator::generate_rook_movemask(c, idx);
}

MoveList MoveGenerator::generate_queen_moves(ChessBoard& c, int side) {
    uint64_t queen_mask = c.get_queen_occupancy(side);
    MoveList to_return;
    while (queen_mask) {
        int queen_idx = pop_min_bit(&queen_mask);
        uint64_t queen_moves = MoveGenerator::generate_queen_movemask(c, queen_idx);
        to_return.add_moves(filter_to_valid_moves(c, side, queen_moves, queen_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_bishop_moves(ChessBoard& c, int side) {
    uint64_t bishop_mask = c.get_bishop_occupancy(side);
    MoveList to_return;
    while (bishop_mask) {
        int bishop_idx = pop_min_bit(&bishop_mask);
        uint64_t bishop_moves = MoveGenerator::generate_bishop_movemask(c, bishop_idx);
        to_return.add_moves(filter_to_valid_moves(c, side, bishop_moves, bishop_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_knight_moves(ChessBoard& c, int side) {
    uint64_t knight_mask = c.get_knight_occupancy(side);
    MoveList to_return;
    while (knight_mask) {
        int knight_idx = pop_min_bit(&knight_mask);
        uint64_t knight_moves = knightMoves[knight_idx];
        to_return.add_moves(filter_to_valid_moves(c, side, knight_moves, knight_idx));
    }
    return to_return;
}

MoveList MoveGenerator::generate_rook_moves(ChessBoard& c, int side) {
    uint64_t rook_mask = c.get_rook_occupancy(side);
    MoveList to_return;
    while (rook_mask) {
        int rook_idx = pop_min_bit(&rook_mask);
        uint64_t rook_moves = MoveGenerator::generate_bishop_movemask(c, rook_idx);
        to_return.add_moves(filter_to_valid_moves(c, side, rook_moves, rook_idx));
    }
    return to_return;
}