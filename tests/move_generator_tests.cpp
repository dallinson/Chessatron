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

TEST(MoveGeneratorTests, CheckCorrectMoveCountMaxMoves) {
    ChessBoard c;
    c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    auto lst = MoveGenerator::generate_moves(c, 0);
    printf("Rook: %lld\n", MoveGenerator::generate_rook_moves(c, 0).len());
    printf("Bishop: %lld\n", MoveGenerator::generate_bishop_moves(c, 0).len());
    print_bitboard(MoveGenerator::generate_bishop_movemask(c, 5));
    for (size_t i = 0; i < MoveGenerator::generate_bishop_moves(c, 0).len(); i++) {
        printf("%s\n", MoveGenerator::generate_bishop_moves(c, 0)[i].to_string().c_str());
    }
    ASSERT_EQ(lst.len(), 218);
}