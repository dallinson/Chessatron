#include <gtest/gtest.h>

#include <cstdint>

#include "../src/utils.hpp"

TEST(UtilTests, TestTrailingZeroBits) {
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(bitboard_to_idx(1ULL << i), i);
    }
}