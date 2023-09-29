#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"

uint64_t perft(ChessBoard& c, MoveHistory& m, int depth, const int side, const bool print_debug = false) {
    MoveList moves;
    uint64_t to_return = 0;

    moves = MoveGenerator::generate_legal_moves(c, side);

    if (depth == 1) {
        if (print_debug) [[unlikely]] {
            for (size_t i = 0; i < moves.len(); i++) {
                printf("%s: 1\n", moves[i].to_string().c_str());
            }
        }
        return moves.len();
    }

    for (size_t i = 0; i < moves.len(); i++) {
        uint64_t val;
        c.make_move(moves[i], m);
        val = perft(c, m, depth - 1, (side + 1) & 1);
        if (print_debug) [[unlikely]] {
            printf("%s: %lu\n", moves[i].to_string().c_str(), val);
        }
        to_return += val;
        c.unmake_move(m);
    }
    return to_return;
}

TEST(PerftTests, TestStartPos) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
    ASSERT_EQ(perft(c, m, 1, 0), 20);
    ASSERT_EQ(perft(c, m, 2, 0), 400);
    ASSERT_EQ(perft(c, m, 3, 0), 8902);
    ASSERT_EQ(perft(c, m, 4, 0), 197281);
    ASSERT_EQ(perft(c, m, 5, 0), 4865609);
    ASSERT_EQ(perft(c, m, 6, 0), 119060324);
}

TEST(PerftTests, TestKiwipete) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    ASSERT_EQ(perft(c, m, 1, 0), 48);
    ASSERT_EQ(perft(c, m, 2, 0), 2039);
    ASSERT_EQ(perft(c, m, 3, 0), 97862);
    ASSERT_EQ(perft(c, m, 4, 0), 4085603);
    ASSERT_EQ(perft(c, m, 5, 0), 193690690);
}

TEST(PerftTests, TestPosition3) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    ASSERT_EQ(perft(c, m, 1, 0), 14);
    ASSERT_EQ(perft(c, m, 2, 0), 191);
    ASSERT_EQ(perft(c, m, 3, 0), 2812);
    ASSERT_EQ(perft(c, m, 4, 0), 43238);
    ASSERT_EQ(perft(c, m, 5, 0), 674624);
    ASSERT_EQ(perft(c, m, 6, 0), 11030083);
}

TEST(PerftTests, TestPosition4) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    ASSERT_EQ(perft(c, m, 1, 0), 6);
    ASSERT_EQ(perft(c, m, 2, 0), 264);
    ASSERT_EQ(perft(c, m, 3, 0), 9467);
    ASSERT_EQ(perft(c, m, 4, 0), 422333);
    ASSERT_EQ(perft(c, m, 5, 0), 15833292);
}

TEST(PerftTests, TestPosition5) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    ASSERT_EQ(perft(c, m, 1, 0), 44);
    ASSERT_EQ(perft(c, m, 2, 0), 1486);
    ASSERT_EQ(perft(c, m, 3, 0), 62379);
    ASSERT_EQ(perft(c, m, 4, 0), 2103487);
    ASSERT_EQ(perft(c, m, 5, 0), 89941194);
}
