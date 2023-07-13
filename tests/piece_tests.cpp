#include <gtest/gtest.h>

#include "../src/pieces.hpp"

TEST(PieceTests, TestPieceGetters) {
    Piece p(0x55);

    ASSERT_EQ(p.get_value(), 0x55);
    ASSERT_EQ(p.get_piece_value(), 0x05);
    ASSERT_EQ(p.get_side(), 0);
    ASSERT_EQ(p.to_bitboard_idx(), 8);
}