#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/search.hpp"

std::vector<std::string> split_on_whitespace(const std::string& data);
void process_position_command(const std::string& line, SearchHandler& s);

TEST(ParsingTests, TestSetString) {
    ChessBoard o;
    BoardHistory hist;
    SearchHandler s;
    process_position_command("position fen startpos", s);
    o.set_from_fen("startpos");
    ASSERT_EQ(s.get_board(), o);

    process_position_command("position fen 8/8/4p2P/Q7/2pk4/8/KPq5/8 w - - 0 1", s);
    o.set_from_fen("8/8/4p2P/Q7/2pk4/8/KPq5/8 w - - 0 1");
    ASSERT_EQ(s.get_board(), o);
}

TEST(ParsingTests, TestMakeMove) {
    ChessBoard o;
    BoardHistory hist;
    SearchHandler s;
    process_position_command("position startpos moves e2e4 d7d5", s);
    o.set_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 1");
    ASSERT_EQ(s.get_board(), o);
    ASSERT_EQ(s.get_history().len(), 3);
}