#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"

TEST(MoveGeneratorTests, CheckCorrectMoveCountStartPos) {
    ChessBoard c;
    c.set_from_fen("startpos");

    auto lst = MoveGenerator::generate_moves(c, 0);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, 0).len(), 4);
}
#include "../src/magic_numbers.hpp"
TEST(MoveGeneratorTests, CheckCorrectMoveCountMaxMoves) {
    ChessBoard c;
    c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    auto rook_moves = MoveGenerator::generate_rook_moves(c, 0);
    ASSERT_EQ(rook_moves.len(), 19);

    auto lst = MoveGenerator::generate_moves(c, 0);
    print_bitboard(MoveGenerator::generate_rook_movemask(c, 8));
    ASSERT_EQ(lst.len(), 218);
}