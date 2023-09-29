#include <gtest/gtest.h>

#include <cstdint>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"

TEST(ChessboardTests, SetFenStartPos) {
    ChessBoard c;
    ASSERT_TRUE(c.set_from_fen("startpos"));

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

    ASSERT_EQ(c.get_side_to_move(), 0);
    ASSERT_EQ(c.get_en_passant_file(), 9);
}

TEST(ChessBoardTests, SetFenExplicitStartPos) {
    ChessBoard c;
    ASSERT_TRUE(c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

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

    ASSERT_EQ(c.get_side_to_move(), 0);
    ASSERT_EQ(c.get_en_passant_file(), 9);
}

TEST(ChessBoardTests, SetFenExplicitStartPosShort) {
    ChessBoard c;
    ASSERT_FALSE(c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR "));

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

TEST(ChessBoardTests, CheckCorrectScoreEvaluations) {
    ChessBoard c;
    c.set_from_fen("5B2/6P1/1p6/8/1N6/kP6/2K5/8 w - -");
    ASSERT_EQ(c.get_score(0), 9);
    ASSERT_EQ(c.get_score(1), 1);
}

TEST(ChessBoardTests, TestMakeUnmakeMove) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    auto BishopMoves = MoveGenerator::generate_pseudolegal_moves(c, 0);
    MoveHistory h;
    for (size_t j = 0; j < BishopMoves.len(); j++) {
        Move m = BishopMoves[j];
        ASSERT_EQ(h.len(), 0);
        c.make_move(m, h);
        ASSERT_EQ(h.len(), 1);
        c.unmake_move(h);
        for (int i = 0; i < 64; i++) {
            ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value())
                << "Mismatch at piece " << std::to_string(i) << " after move " << m.to_string() << " with flags "
                << std::to_string(m.get_move_flags()) << " (value " << std::to_string(m.get_move()) << ")";
        }
    }

    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    c.make_move(Move(DOUBLE_PAWN_PUSH, 24, 8), h);
    c.unmake_move(h);
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }

    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    c.make_move(Move(6322), h);
    c.unmake_move(h);
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
}

TEST(ChessBoardTests, TestMakeMove) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1");
    MoveHistory m;
    c.make_move(Move(QUIET_MOVE, 17, 9), m);
    ASSERT_STREQ(Move(QUIET_MOVE, 17, 9).to_string().c_str(), "b2b3");
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    c.make_move(Move(DOUBLE_PAWN_PUSH, 24, 8), m);
    ASSERT_STREQ(Move(DOUBLE_PAWN_PUSH, 24, 8).to_string().c_str(), "a2a4");
    ASSERT_EQ(Move(DOUBLE_PAWN_PUSH, 24, 8).get_move(), 5640);
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
}

TEST(ChessBoardTests, TestMakePromotion) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1");
    o.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/P2P2PP/b2Q1R1K w kq - 0 2");
    MoveHistory m;
    c.make_move(Move(BISHOP_PROMOTION_CAPTURE, 0, 9), m);
    ASSERT_EQ(c, o);
}