#include <gtest/gtest.h>

#include "../src/evaluation.hpp"


TEST(EvaluationTests, CheckCorrectScoreEvaluations) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("5B2/6P1/1p6/8/1N6/kP6/2K5/8 w - - 0 1");
    ASSERT_EQ(Evaluation::evaluate_board(c, Side::WHITE), 1065);
    ASSERT_EQ(Evaluation::evaluate_board(c, Side::BLACK), 75);
    ASSERT_EQ(Evaluation::evaluate_board(c), 990);
}

TEST(EvaluationTests, TestEndgame) {
    ChessBoard c;
    c.set_from_fen("startpos");
    ASSERT_FALSE(Evaluation::is_endgame(c));

    c.set_from_fen("rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
    ASSERT_TRUE(Evaluation::is_endgame(c));
    c.set_from_fen("8/1k6/8/8/6K1/8/1q6/8 w - - 0 1");
    ASSERT_TRUE(Evaluation::is_endgame(c));
    c.set_from_fen("8/1k6/8/8/6K1/8/1q1b4/8 w - - 0 1");
    ASSERT_TRUE(Evaluation::is_endgame(c));
    c.set_from_fen("8/1k6/8/8/6K1/8/1q1n4/8 w - - 0 1");
    ASSERT_TRUE(Evaluation::is_endgame(c));
    c.set_from_fen("8/1k6/8/8/6K1/8/1q1p4/8 w - - 0 1");
    ASSERT_FALSE(Evaluation::is_endgame(c));

}