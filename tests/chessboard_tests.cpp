#include <gtest/gtest.h>

#include <cstdint>

#include "../src/chessboard.hpp"

TEST(ChessboardTests, SetFenStartPos) {
    ChessBoard c;
    c.set_from_fen("startpos");

    ASSERT_EQ(c.get_occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.get_knight_occupancy(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.get_queen_occupancy(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.get_king_occupancy(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.get_side_occupancy(0), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(0), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(0), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.get_knight_occupancy(0), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(0), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.get_queen_occupancy(0), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.get_king_occupancy(0), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.get_side_occupancy(1), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.get_pawn_occupancy(1), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.get_rook_occupancy(1), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.get_knight_occupancy(1), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.get_bishop_occupancy(1), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.get_queen_occupancy(1), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.get_king_occupancy(1), (uint64_t) 0x1000000000000000);

}

TEST(ChessBoardTests, SetFenExplicitStartPos) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ASSERT_EQ(c.get_occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.get_knight_occupancy(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.get_queen_occupancy(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.get_king_occupancy(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.get_side_occupancy(0), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(0), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(0), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.get_knight_occupancy(0), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(0), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.get_queen_occupancy(0), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.get_king_occupancy(0), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.get_side_occupancy(1), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.get_pawn_occupancy(1), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.get_rook_occupancy(1), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.get_knight_occupancy(1), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.get_bishop_occupancy(1), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.get_queen_occupancy(1), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.get_king_occupancy(1), (uint64_t) 0x1000000000000000);
}

TEST(ChessBoardTests, SetFenExplicitStartPosShort) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR ");

    ASSERT_EQ(c.get_occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.get_knight_occupancy(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.get_queen_occupancy(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.get_king_occupancy(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.get_side_occupancy(0), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(0), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(0), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.get_knight_occupancy(0), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(0), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.get_queen_occupancy(0), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.get_king_occupancy(0), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.get_side_occupancy(1), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.get_pawn_occupancy(1), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.get_rook_occupancy(1), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.get_knight_occupancy(1), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.get_bishop_occupancy(1), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.get_queen_occupancy(1), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.get_king_occupancy(1), (uint64_t) 0x1000000000000000);
}