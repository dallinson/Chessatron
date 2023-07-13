#include <gtest/gtest.h>

#include <cstdint>

#include "../src/utils.hpp"

TEST(UtilTests, TestTrailingZeroBits) {
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(bitboard_to_idx(1ULL << i), i);
    }
}

TEST(UtilTests, TestBitboardFromIdx) {
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(idx_to_bitboard(i), 1ULL << i);
    }
}

TEST(UtilTests, TestPrintBitboard) {
    testing::internal::CaptureStdout();
    print_bitboard(0);
    std::string from_stdout = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("........\n........\n........\n........\n........\n........\n........\n........\n", from_stdout.c_str());

    testing::internal::CaptureStdout();
    print_bitboard(0xFFFF00000000FFFF);
    from_stdout = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("########\n########\n........\n........\n........\n........\n########\n########\n", from_stdout.c_str());
}