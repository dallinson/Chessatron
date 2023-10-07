#include <gtest/gtest.h>

#include <iostream>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"
#include "../src/search.hpp"

TEST(PerftTests, TestStartPos) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
    ASSERT_EQ(Perft::run_perft(c, 1), 20);
    ASSERT_EQ(Perft::run_perft(c, 2), 400);
    ASSERT_EQ(Perft::run_perft(c, 3), 8902);
    ASSERT_EQ(Perft::run_perft(c, 4), 197281);
    ASSERT_EQ(Perft::run_perft(c, 5), 4865609);
    ASSERT_EQ(Perft::run_perft(c, 6), 119060324);
}

TEST(PerftTests, TestKiwipete) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    ASSERT_EQ(Perft::run_perft(c, 1), 48);
    ASSERT_EQ(Perft::run_perft(c, 2), 2039);
    ASSERT_EQ(Perft::run_perft(c, 3), 97862);
    ASSERT_EQ(Perft::run_perft(c, 4), 4085603);
    ASSERT_EQ(Perft::run_perft(c, 5), 193690690);
}

TEST(PerftTests, TestPosition3) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    ASSERT_EQ(Perft::run_perft(c, 1), 14);
    ASSERT_EQ(Perft::run_perft(c, 2), 191);
    ASSERT_EQ(Perft::run_perft(c, 3), 2812);
    ASSERT_EQ(Perft::run_perft(c, 4), 43238);
    ASSERT_EQ(Perft::run_perft(c, 5), 674624);
    ASSERT_EQ(Perft::run_perft(c, 6), 11030083);
}

TEST(PerftTests, TestPosition4) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    ASSERT_EQ(Perft::run_perft(c, 1), 6);
    ASSERT_EQ(Perft::run_perft(c, 2), 264);
    ASSERT_EQ(Perft::run_perft(c, 3), 9467);
    ASSERT_EQ(Perft::run_perft(c, 4), 422333);
    ASSERT_EQ(Perft::run_perft(c, 5), 15833292);
}

TEST(PerftTests, TestPosition5) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    ASSERT_EQ(Perft::run_perft(c, 1), 44);
    ASSERT_EQ(Perft::run_perft(c, 2), 1486);
    ASSERT_EQ(Perft::run_perft(c, 3), 62379);
    ASSERT_EQ(Perft::run_perft(c, 4), 2103487);
    ASSERT_EQ(Perft::run_perft(c, 5), 89941194);
}
