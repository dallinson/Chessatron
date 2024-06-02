#include <gtest/gtest.h>

#include <cstdint>

#include "../src/chessboard.hpp"
#include "../src/utils.hpp"

TEST(UtilTests, TestTrailingZeroBits) {
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(get_lsb(1ULL << i), static_cast<Square>(i));
    }

    Position pos;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K3 w Qkq - 0 1");
    ASSERT_EQ(get_lsb(pos.kings(Side::WHITE)), Square::E1);
}

TEST(UtilTests, TestBitboardFromIdx) {
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(idx_to_bb(i), 1ULL << i);
    }
}

TEST(UtilTests, TestPrintBitboard) {
    testing::internal::CaptureStdout();
    print_bb(0);
    std::string from_stdout = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("........\n........\n........\n........\n........\n........\n........\n........\n", from_stdout.c_str());

    testing::internal::CaptureStdout();
    print_bb(0xFFFF00000000FFFF);
    from_stdout = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("########\n########\n........\n........\n........\n........\n########\n########\n", from_stdout.c_str());
}