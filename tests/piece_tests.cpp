#include <gtest/gtest.h>

#include "../src/pieces.hpp"

TEST(PieceTests, TestPieceGetters) {
    Piece p(0x55);

    ASSERT_EQ(p.get_value(), 0x55);
    ASSERT_EQ(p.get_type(), PieceValues::QUEEN);
    ASSERT_EQ(p.get_side(), Side::WHITE);
    ASSERT_EQ(p.to_bitboard_idx(), 8);
}