#include <gtest/gtest.h>

#include "../src/chessboard.hpp"

std::vector<std::string> split_on_whitespace(const std::string& data);
void process_position_command(const std::string& line, ChessBoard& c, MoveHistory& m);

TEST(ParsingTests, TestSetString) {
    ChessBoard c, o;
    MoveHistory m;
    process_position_command("position fen startpos", c, m);
    o.set_from_fen("startpos");
    ASSERT_EQ(c, o);

    process_position_command("position fen 8/8/4p2P/Q7/2pk4/8/KPq5/8 w - - 0 1", c, m);
    o.set_from_fen("8/8/4p2P/Q7/2pk4/8/KPq5/8 w - - 0 1");
    ASSERT_EQ(c, o);
}

TEST(ParsingTests, TestMakeMove) {
    ChessBoard c, o;
    MoveHistory m;
    process_position_command("position startpos moves e2e4 d7d5", c, m);
    o.set_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 1");
    ASSERT_EQ(c, o);
    ASSERT_EQ(m.len(), 2);
}