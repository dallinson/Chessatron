#include <gtest/gtest.h>

#include "../src/move.hpp"

TEST(MoveTests, TestPromotionToString) {
    Move rook_promotion = Move(MoveFlags::ROOK_PROMOTION, 56, 48);
    Move rook_promotion_capture = Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48);
    ASSERT_STREQ(rook_promotion.to_string().c_str(), "a7a8r");
    ASSERT_STREQ(rook_promotion_capture.to_string().c_str(), "a7b8r");

    Move knight_promotion = Move(MoveFlags::KNIGHT_PROMOTION, 56, 48);
    Move knight_promotion_capture = Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, 57, 48);
    ASSERT_STREQ(knight_promotion.to_string().c_str(), "a7a8n");
    ASSERT_STREQ(knight_promotion_capture.to_string().c_str(), "a7b8n");

    Move bishop_promotion = Move(MoveFlags::BISHOP_PROMOTION, 56, 48);
    Move bishop_promotion_capture = Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 57, 48);
    ASSERT_STREQ(bishop_promotion.to_string().c_str(), "a7a8b");
    ASSERT_STREQ(bishop_promotion_capture.to_string().c_str(), "a7b8b");

    Move queen_promotion = Move(MoveFlags::QUEEN_PROMOTION, 56, 48);
    Move queen_promotion_capture = Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, 57, 48);
    ASSERT_STREQ(queen_promotion.to_string().c_str(), "a7a8q");
    ASSERT_STREQ(queen_promotion_capture.to_string().c_str(), "a7b8q");
}