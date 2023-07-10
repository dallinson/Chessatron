#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"

TEST(MoveGeneratorTests, CheckCorrectMoveCount) {
    ChessBoard c;
    c.set_from_fen("startpos");

    auto lst = MoveGenerator::generate_moves(c, 0);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, 0).len(), 4);
}