#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/search.hpp"

TEST(SearchTests, TestThreefoldDetection) {
    Position pos;
    pos.set_from_fen("rnbqkbnr/1pppppp1/p6p/8/6P1/7N/PPPPPP1P/RNBQKB1R w KQkq - 0 3");

    auto board_hist = BoardHistory(pos);
    pos = pos.make_move(pos.generate_move_from_string("c2c4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e7e5").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("b1c3").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("b8c6").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("e2e3").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("f8c5").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("f1g2").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("c5b6").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("d2d4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e5d4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e3d4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("b6d4").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("e1g1").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("g8e7").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("c3d5").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("d4e5").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("f2f4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e5d4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("c1e3").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("d4e3").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("d5e3").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("d7d6").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("f4f5").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e7g8").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("h3f4").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("g8f6").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e3d5").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("e8g8").value(), board_hist);
    
    pos = pos.make_move(pos.generate_move_from_string("h2h3").value(), board_hist);
    pos = pos.make_move(pos.generate_move_from_string("a8b8").value(), board_hist);

    pos = pos.make_move(pos.generate_move_from_string("b2b3").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("f6d5").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("f4d5").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("c6e7").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("d5f4").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("e7c6").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("f4d5").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("c6e7").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("d5f4").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("e7c6").value(), board_hist);
    ASSERT_FALSE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
    pos = pos.make_move(pos.generate_move_from_string("f4d5").value(), board_hist);
    ASSERT_TRUE(Search::is_threefold_repetition(board_hist, pos.get_halfmove_clock(), pos.zobrist_key()));
}