#include <gtest/gtest.h>

#include "../src/pieces.hpp"

TEST(PieceTests, TestPieceGetters) {
    Piece p(0x55);

    ASSERT_EQ(p.get_value(), 0x55);
    ASSERT_EQ(p.type(), PieceTypes::QUEEN);
    ASSERT_EQ(p.side(), Side::WHITE);
    ASSERT_EQ(p.to_bitboard_idx(), 8);
}