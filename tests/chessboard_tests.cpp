#include <gtest/gtest.h>

#include <cstdint>

#include "../src/chessboard.hpp"
#include "../src/evaluation.hpp"
#include "../src/move_generator.hpp"

TEST(ChessBoardTests, SetFenStartPos) {
    ChessBoard c;
    ASSERT_TRUE(c.set_from_fen("startpos").has_value());

    ASSERT_EQ(c.get_occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.get_knight_occupancy(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.get_queen_occupancy(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.get_king_occupancy(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.get_side_occupancy(Side::WHITE), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(Side::WHITE), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(Side::WHITE), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.get_knight_occupancy(Side::WHITE), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(Side::WHITE), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.get_queen_occupancy(Side::WHITE), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.get_king_occupancy(Side::WHITE), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.get_side_occupancy(Side::BLACK), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.get_pawn_occupancy(Side::BLACK), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.get_rook_occupancy(Side::BLACK), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.get_knight_occupancy(Side::BLACK), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.get_bishop_occupancy(Side::BLACK), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.get_queen_occupancy(Side::BLACK), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.get_king_occupancy(Side::BLACK), (uint64_t) 0x1000000000000000);

    ASSERT_EQ(c.get_side_to_move(), Side::WHITE);
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

    ASSERT_EQ(c.get_side_occupancy(Side::WHITE), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(Side::WHITE), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(Side::WHITE), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.get_knight_occupancy(Side::WHITE), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(Side::WHITE), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.get_queen_occupancy(Side::WHITE), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.get_king_occupancy(Side::WHITE), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.get_side_occupancy(Side::BLACK), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.get_pawn_occupancy(Side::BLACK), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.get_rook_occupancy(Side::BLACK), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.get_knight_occupancy(Side::BLACK), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.get_bishop_occupancy(Side::BLACK), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.get_queen_occupancy(Side::BLACK), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.get_king_occupancy(Side::BLACK), (uint64_t) 0x1000000000000000);

    ASSERT_EQ(c.get_side_to_move(), Side::WHITE);
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

    ASSERT_EQ(c.get_side_occupancy(Side::WHITE), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.get_pawn_occupancy(Side::WHITE), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.get_rook_occupancy(Side::WHITE), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.get_knight_occupancy(Side::WHITE), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.get_bishop_occupancy(Side::WHITE), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.get_queen_occupancy(Side::WHITE), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.get_king_occupancy(Side::WHITE), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.get_side_occupancy(Side::BLACK), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.get_pawn_occupancy(Side::BLACK), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.get_rook_occupancy(Side::BLACK), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.get_knight_occupancy(Side::BLACK), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.get_bishop_occupancy(Side::BLACK), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.get_queen_occupancy(Side::BLACK), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.get_king_occupancy(Side::BLACK), (uint64_t) 0x1000000000000000);
}

TEST(ChessBoardTests, TestMakeUnmakeMove) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    const auto original_midgame_score = c.get_midgame_score(Side::WHITE);
    const auto original_endgame_score = c.get_endgame_score(Side::WHITE);
    auto BishopMoves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    MoveHistory h;
    for (size_t j = 0; j < BishopMoves.len(); j++) {
        Move m = BishopMoves[j].move;
        ASSERT_EQ(h.len(), 0);
        c.make_move(m, h);
        ASSERT_EQ(h.len(), 1);
        c.unmake_move(h);
        ASSERT_EQ(original_midgame_score, c.get_midgame_score(Side::WHITE)) << "Midgame score mismatch after move " << m.to_string();
        ASSERT_EQ(original_endgame_score, c.get_endgame_score(Side::WHITE)) << "Endgame score mismatch after move " << m.to_string();
        for (int i = 0; i < 64; i++) {
            ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value())
                << "Mismatch at piece " << std::to_string(i) << " after move " << m.to_string() << " with flags "
                << std::to_string(static_cast<int>(m.get_move_flags())) << " (value " << std::to_string(m.get_move()) << ")";
        }
    }

    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8), h);
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
    c.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), m);
    ASSERT_STREQ(Move(MoveFlags::QUIET_MOVE, 17, 9).to_string().c_str(), "b2b3");
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8), m);
    ASSERT_STREQ(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8).to_string().c_str(), "a2a4");
    ASSERT_EQ(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8).get_move(), 5640);
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i).get_value(), o.get_piece(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
}

TEST(ChessBoardTests, TestMakePromotion) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1");
    o.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/P2P2PP/b2Q1R1K w kq - 0 2");
    MoveHistory m;
    c.make_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 0, 9), m);
    ASSERT_EQ(c, o);
}

TEST(ChessBoardTests, TestEquality) {
    ChessBoard c, o;
    c.set_from_fen("startpos");
    o.set_from_fen("startpos");
    ASSERT_EQ(c, o);
}

TEST(ChessBoardTests, TestClearBoard) {
    ChessBoard c;
    c.set_from_fen("startpos");
    c.clear_board();
    for (int i = 0; i < 12; i++) {
        ASSERT_EQ(c.get_bitboard(i), 0);
    }
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.get_piece(i), Piece(0));
    }
    ASSERT_EQ(c.get_side_to_move(), Side::WHITE);
    ASSERT_EQ(c.get_en_passant_file(), 9);
    ASSERT_FALSE(c.get_kingside_castling(Side::WHITE));
    ASSERT_FALSE(c.get_kingside_castling(Side::BLACK));
    ASSERT_FALSE(c.get_queenside_castling(Side::WHITE));
    ASSERT_FALSE(c.get_queenside_castling(Side::BLACK));
}

TEST(ChessBoardTests, TestPrintBoard) {
    ChessBoard c;
    c.set_from_fen("startpos");
    testing::internal::CaptureStdout();
    c.print_board();
    std::string from_stdout = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("rnbqkbnr\npppppppp\n........\n........\n........\n........\nPPPPPPPP\nRNBQKBNR\n", from_stdout.c_str());
}

TEST(ChessBoardTests, TestPromotionsFromString) {
    ChessBoard c;
    c.set_from_fen("1r6/P7/5k2/8/5K2/8/8/8 w - - 0 1");
    ASSERT_EQ(c.generate_move_from_string("a7a8r").value(), Move(MoveFlags::ROOK_PROMOTION, 56, 48));
    ASSERT_EQ(c.generate_move_from_string("a7a8n").value(), Move(MoveFlags::KNIGHT_PROMOTION, 56, 48));
    ASSERT_EQ(c.generate_move_from_string("a7a8b").value(), Move(MoveFlags::BISHOP_PROMOTION, 56, 48));
    ASSERT_EQ(c.generate_move_from_string("a7a8q").value(), Move(MoveFlags::QUEEN_PROMOTION, 56, 48));

    ASSERT_EQ(c.generate_move_from_string("a7b8r").value(), Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48));
    ASSERT_EQ(c.generate_move_from_string("a7b8n").value(), Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, 57, 48));
    ASSERT_EQ(c.generate_move_from_string("a7b8b").value(), Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 57, 48));
    ASSERT_EQ(c.generate_move_from_string("a7b8q").value(), Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, 57, 48));
}

TEST(ChessBoardTests, TestInvalidMoveString) {
    ChessBoard c;
    c.set_from_fen("startpos");
    ASSERT_FALSE(c.generate_move_from_string("dfjkgsjkfdj").has_value());
    ASSERT_FALSE(c.generate_move_from_string("a7a8c").has_value());
}

TEST(ChessBoardTests, TestEnPassantFromString) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    ASSERT_EQ(c.generate_move_from_string("d4e3"), Move(MoveFlags::EN_PASSANT_CAPTURE, 20, 27));
    ASSERT_EQ(c.generate_move_from_string("c2c4"), Move(MoveFlags::DOUBLE_PAWN_PUSH, 26, 10));
}

TEST(ChessBoardTests, TestCastlingFromString) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    ASSERT_EQ(c.generate_move_from_string("e1c1"), Move(MoveFlags::QUEENSIDE_CASTLE, 2, 4));
    ASSERT_EQ(c.generate_move_from_string("e1g1"), Move(MoveFlags::KINGSIDE_CASTLE, 6, 4));
    ASSERT_EQ(c.generate_move_from_string("e1f1"), Move(MoveFlags::QUIET_MOVE, 5, 4));
    // not a castling move but included just to cover this branch
}

TEST(ChessBoardTests, TestMakeNullMove) {
    ChessBoard c, o;
    MoveHistory m;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 2");
    c.make_move(0, m);
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    o.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    c.make_move(0, m);
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR b Kkq - 0 2");
    c.make_move(0, m);
    ASSERT_EQ(c, o);
}

TEST(ChessBoardTests, TestUnmakeNullMove) {
    ChessBoard c, o;
    MoveHistory m;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    c.make_move(0, m);
    c.unmake_move(m);
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    o.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    c.make_move(0, m);
    c.unmake_move(m);
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    c.make_move(0, m);
    c.unmake_move(m);
    ASSERT_EQ(c, o);
}

TEST(ChessBoardTests, TestHalfmoveClock) {
    ChessBoard c;
    c.set_from_fen("startpos");
    ASSERT_EQ(c.get_halfmove_clock(), 0);

    MoveHistory m;
    c.make_move(Move::NULL_MOVE, m);
    ASSERT_EQ(c.get_halfmove_clock(), 1);
    c.unmake_move(m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);

    c.make_move(Move::NULL_MOVE, m);
    c.make_move(Move::NULL_MOVE, m);
    c.make_move(Move::NULL_MOVE, m);
    ASSERT_EQ(c.get_halfmove_clock(), 3);
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 35, 51), m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c.unmake_move(m);
    ASSERT_EQ(c.get_halfmove_clock(), 3);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 43, 51), m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c.unmake_move(m);
    ASSERT_EQ(c.get_halfmove_clock(), 3);
    // Both pawn moves reset it

    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 24 48");
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    c.make_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 0, 9), m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c.unmake_move(m);
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    c.make_move(Move(MoveFlags::BISHOP_PROMOTION, 1, 9), m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c.unmake_move(m);
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    // Test promotions reset clock

    c.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 24 48");
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    const auto original_midgame_score = c.get_midgame_score(Side::WHITE);
    const auto original_endgame_score = c.get_endgame_score(Side::WHITE);
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), m);
    c.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c.unmake_move(m);
    c.unmake_move(m);
    ASSERT_EQ(original_midgame_score, c.get_midgame_score(Side::WHITE));
    ASSERT_EQ(original_endgame_score, c.get_endgame_score(Side::WHITE));
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    // en passant captures are correctly handled

    c.set_from_fen("2r5/6k1/8/6K1/8/8/2Q5/8 w - - 12 24");
    ASSERT_EQ(c.get_halfmove_clock(), 12);
    c.make_move(Move(MoveFlags::CAPTURE, 58, 10), m);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c.unmake_move(m);
    ASSERT_EQ(c.get_halfmove_clock(), 12);
    // and tests normal captures reset it
}

TEST(ChessBoardTests, TestUnmakeScores) {
    ChessBoard c;
    MoveHistory m;
    c.set_from_fen("1r4k1/P7/8/3Pp3/8/1b6/P7/R3K2R w KQ e6 0 1");
    const auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    const auto mg_score = c.get_midgame_score(Side::WHITE);
    const auto eg_score = c.get_endgame_score(Side::WHITE);
    for (size_t i = 0; i < moves.len(); i++) {
        c.make_move(moves[i].move, m);
        c.unmake_move(m);
        ASSERT_EQ(c.get_midgame_score(Side::WHITE), mg_score) << "Midgame score mismatch on move " << moves[i].move.to_string();
        ASSERT_EQ(c.get_endgame_score(Side::WHITE), eg_score) << "Endgame score mismatch on move " << moves[i].move.to_string();
    }
}