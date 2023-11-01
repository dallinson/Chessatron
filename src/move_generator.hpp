#pragma once

#include <cstdint>

#include "chessboard.hpp"
#include "move.hpp"
#include "utils.hpp"

namespace MoveGenerator {
    MoveList generate_legal_moves(const ChessBoard& c, const Side side);
    int get_checking_piece_count(const ChessBoard& c, const Side side, const int king_idx);
    Bitboard get_checkers(const ChessBoard& c, const Side side, const int king_idx);

    Bitboard generate_bishop_movemask(const Bitboard b, const int idx);
    Bitboard generate_rook_movemask(const Bitboard b, const int idx);
    Bitboard generate_queen_movemask(const Bitboard b, const int idx);
    Bitboard generate_movemask(const PieceTypes piece_type, const Bitboard b, const int idx);

    template <PieceTypes piece_type> void generate_moves(const ChessBoard& c, const Side side, MoveList& move_list);
    void generate_pawn_moves(const ChessBoard& c, const Side side, MoveList& move_list);
    void generate_castling_moves(const ChessBoard& c, const Side side, MoveList& move_list);

    void filter_to_pseudolegal_moves(const Bitboard friendlies, const Bitboard enemies, const Bitboard potential_moves, const int idx,
                                     MoveList& move_list);
    MoveList filter_to_legal_moves(const ChessBoard& c, const MoveList& move_list);

    MoveList generate_pseudolegal_moves(const ChessBoard& c, const Side side);

    bool is_move_legal(const ChessBoard& c, const Move m);
    bool is_move_pseudolegal(const ChessBoard& c, const Move to_test);
} // namespace MoveGenerator
