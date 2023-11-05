#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"
#include "../src/move_ordering.hpp"

TEST(MoveOrderingTests, TestReorderCaptures) {
    ChessBoard c;
    c.set_from_fen("6kr/pp2r2p/n1p1PB1Q/2q5/2B4P/2N3p1/PPP3P1/7K w - - 1 0");
    // Taken from https://wtharvey.com/m8n4.txt, Serafino Dubois vs Augustus Mongredien, London, 1862
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, c.get_side_to_move());
    MoveOrdering::reorder_captures_first(moves, 0);
    for (size_t i = 0; i < 4; i++) {
        ASSERT_TRUE(moves[i].is_capture());
    }
    for (size_t i = 4; i < moves.len(); i++) {
        ASSERT_FALSE(moves[i].is_capture());
    }
}