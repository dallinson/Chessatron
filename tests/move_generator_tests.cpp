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

    lst = MoveGenerator::generate_moves(c, 1);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 1).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, 1).len(), 4);
}

TEST(MoveGeneratorTests, CheckCorrectMoveCountMaxMoves) {
    ChessBoard c;
    c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    auto rook_moves = MoveGenerator::generate_rook_moves(c, 0);
    ASSERT_EQ(rook_moves.len(), 19);

    auto lst = MoveGenerator::generate_moves(c, 0);
    ASSERT_EQ(lst.len(), 218);
}

TEST(MoveGeneratorTests, CheckEnPassant) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR ");
    c.set_en_passant_file(4);

    auto pawn_moves = MoveGenerator::generate_pawn_moves(c, 0);
    ASSERT_EQ(pawn_moves.len(), 16);
    FAIL();
}