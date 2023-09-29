#pragma once

#include <cstdint>

#include "chessboard.hpp"
#include "move.hpp"
#include "utils.hpp"

namespace MoveGenerator {
    MoveList generate_legal_moves(ChessBoard& c, const int side);
    MoveList generate_king_moves(const ChessBoard& c, const int side);
    MoveList filter_to_pseudolegal_moves(const ChessBoard& c, const int side, const Bitboard potential_moves, const int idx);
    int get_checking_piece_count(const ChessBoard& c, const int side, const int king_idx);
    Bitboard get_checkers(const ChessBoard& c, const int side, const int king_idx);

    Bitboard generate_bishop_movemask(const Bitboard b, const int idx);
    Bitboard generate_rook_movemask(const Bitboard b, const int idx);
    Bitboard generate_queen_movemask(const Bitboard b, const int idx);

    MoveList generate_queen_moves(const ChessBoard& c, const int side);
    MoveList generate_bishop_moves(const ChessBoard& c, const int side);
    MoveList generate_knight_moves(const ChessBoard& c, const int side);
    MoveList generate_rook_moves(const ChessBoard& c, const int side);
    MoveList generate_pawn_moves(const ChessBoard& c, const int side);

    MoveList generate_castling_moves(const ChessBoard& c, const int side);
    MoveList filter_to_legal_moves(ChessBoard& c, const int side, const MoveList& move_list);

    MoveList generate_pseudolegal_moves(const ChessBoard& c, const int side);

    bool is_move_legal(const ChessBoard& c, const Move m);
} // namespace MoveGenerator
