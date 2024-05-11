#include <gtest/gtest.h>

#include <cstdint>

#include "../src/chessboard.hpp"
#include "../src/evaluation.hpp"
#include "../src/move_generator.hpp"

TEST(ChessBoardTests, SetFenStartPos) {
    ChessBoard c;
    ASSERT_TRUE(c.set_from_fen("startpos").has_value());

    ASSERT_EQ(c.occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.pawns(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.rooks(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.knights(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.bishops(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.queens(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.kings(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.occupancy(Side::WHITE), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.pawns(Side::WHITE), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.rooks(Side::WHITE), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.knights(Side::WHITE), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.bishops(Side::WHITE), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.queens(Side::WHITE), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.kings(Side::WHITE), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.occupancy(Side::BLACK), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.pawns(Side::BLACK), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.rooks(Side::BLACK), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.knights(Side::BLACK), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.bishops(Side::BLACK), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.queens(Side::BLACK), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.kings(Side::BLACK), (uint64_t) 0x1000000000000000);

    ASSERT_EQ(c.get_side_to_move(), Side::WHITE);
    ASSERT_EQ(c.get_en_passant_file(), 9);
}

TEST(ChessBoardTests, SetFenExplicitStartPos) {
    ChessBoard c;
    ASSERT_TRUE(c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    ASSERT_EQ(c.occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.pawns(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.rooks(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.knights(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.bishops(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.queens(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.kings(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.occupancy(Side::WHITE), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.pawns(Side::WHITE), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.rooks(Side::WHITE), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.knights(Side::WHITE), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.bishops(Side::WHITE), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.queens(Side::WHITE), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.kings(Side::WHITE), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.occupancy(Side::BLACK), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.pawns(Side::BLACK), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.rooks(Side::BLACK), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.knights(Side::BLACK), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.bishops(Side::BLACK), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.queens(Side::BLACK), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.kings(Side::BLACK), (uint64_t) 0x1000000000000000);

    ASSERT_EQ(c.get_side_to_move(), Side::WHITE);
    ASSERT_EQ(c.get_en_passant_file(), 9);
}

TEST(ChessBoardTests, SetFenExplicitStartPosShort) {
    ChessBoard c;
    ASSERT_FALSE(c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR "));

    ASSERT_EQ(c.occupancy(), (uint64_t) 0xFFFF00000000FFFF);
    ASSERT_EQ(c.pawns(), (uint64_t) 0x00FF00000000FF00);
    ASSERT_EQ(c.rooks(), (uint64_t) 0x8100000000000081);
    ASSERT_EQ(c.knights(), (uint64_t) 0x4200000000000042);
    ASSERT_EQ(c.bishops(), (uint64_t) 0x2400000000000024);
    ASSERT_EQ(c.queens(), (uint64_t) 0x0800000000000008);
    ASSERT_EQ(c.kings(), (uint64_t) 0x1000000000000010);

    ASSERT_EQ(c.occupancy(Side::WHITE), (uint64_t) 0x000000000000FFFF);
    ASSERT_EQ(c.pawns(Side::WHITE), (uint64_t) 0x000000000000FF00);
    ASSERT_EQ(c.rooks(Side::WHITE), (uint64_t) 0x0000000000000081);
    ASSERT_EQ(c.knights(Side::WHITE), (uint64_t) 0x0000000000000042);
    ASSERT_EQ(c.bishops(Side::WHITE), (uint64_t) 0x0000000000000024);
    ASSERT_EQ(c.queens(Side::WHITE), (uint64_t) 0x0000000000000008);
    ASSERT_EQ(c.kings(Side::WHITE), (uint64_t) 0x0000000000000010);

    ASSERT_EQ(c.occupancy(Side::BLACK), (uint64_t) 0xFFFF000000000000);
    ASSERT_EQ(c.pawns(Side::BLACK), (uint64_t) 0x00FF000000000000);
    ASSERT_EQ(c.rooks(Side::BLACK), (uint64_t) 0x8100000000000000);
    ASSERT_EQ(c.knights(Side::BLACK), (uint64_t) 0x4200000000000000);
    ASSERT_EQ(c.bishops(Side::BLACK), (uint64_t) 0x2400000000000000);
    ASSERT_EQ(c.queens(Side::BLACK), (uint64_t) 0x0800000000000000);
    ASSERT_EQ(c.kings(Side::BLACK), (uint64_t) 0x1000000000000000);
}

TEST(ChessBoardTests, TestMakeUnmakeMove) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    const auto original_score = c.get_score(Side::WHITE);
    auto BishopMoves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    BoardHistory hist(c);
    for (size_t j = 0; j < BishopMoves.size(); j++) {
        Move m = BishopMoves[j].move;
        ASSERT_EQ(hist.len(), 1);
        c = c.make_move(m, hist);
        ASSERT_EQ(hist.len(), 2);
        c = hist.pop_board();
        ASSERT_EQ(original_score, c.get_score(Side::WHITE)) << "Score mismatch after move " << m.to_string();
        for (int i = 0; i < 64; i++) {
            ASSERT_EQ(c.piece_at(i).get_value(), o.piece_at(i).get_value())
                << "Mismatch at piece " << std::to_string(i) << " after move " << m.to_string() << " with flags "
                << std::to_string(static_cast<int>(m.get_move_flags())) << " (value " << std::to_string(m.value()) << ")";
        }
    }

    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    hist = BoardHistory(c);
    c = c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8), hist);
    c = hist.pop_board();
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.piece_at(i).get_value(), o.piece_at(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }

    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    hist = BoardHistory(c);
    c = c.make_move(Move(6322), hist);
    c = hist.pop_board();
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.piece_at(i).get_value(), o.piece_at(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
}

TEST(ChessBoardTests, TestMakeMove) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1");
    BoardHistory hist(c);
    c = c.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), hist);
    ASSERT_STREQ(Move(MoveFlags::QUIET_MOVE, 17, 9).to_string().c_str(), "b2b3");
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.piece_at(i).get_value(), o.piece_at(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    hist = BoardHistory(c);
    c = c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8), hist);
    ASSERT_STREQ(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8).to_string().c_str(), "a2a4");
    ASSERT_EQ(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8).value(), 5640);
    for (int i = 0; i < 64; i++) {
        ASSERT_EQ(c.piece_at(i).get_value(), o.piece_at(i).get_value()) << "Mismatch at piece " << std::to_string(i) << "!";
    }
}

TEST(ChessBoardTests, TestMakePromotion) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1");
    o.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/P2P2PP/b2Q1R1K w kq - 0 2");
    BoardHistory hist(c);
    c = c.make_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 0, 9), hist);
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
        ASSERT_EQ(c.get_bb(i / 2, i % 6), 0);
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
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 2");
    BoardHistory hist(c);
    c = c.make_move(0, hist);
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    o.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    hist = BoardHistory(c);
    c = c.make_move(0, hist);
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR b Kkq - 0 2");
    hist = BoardHistory(c);
    c = c.make_move(0, hist);
    ASSERT_EQ(c, o);
}

TEST(ChessBoardTests, TestUnmakeNullMove) {
    ChessBoard c, o;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardHistory hist(c);
    c = c.make_move(0, hist);
    c = hist.pop_board();
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    o.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    hist = BoardHistory(c);
    c = c.make_move(0, hist);
    c = hist.pop_board();
    ASSERT_EQ(c, o);

    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    o.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    hist = BoardHistory(c);
    c = c.make_move(0, hist);
    c = hist.pop_board();
    ASSERT_EQ(c, o);
}

TEST(ChessBoardTests, TestHalfmoveClock) {
    ChessBoard c;
    c.set_from_fen("startpos");
    ASSERT_EQ(c.get_halfmove_clock(), 0);

    BoardHistory hist(c);
    c = c.make_move(Move::NULL_MOVE(), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 1);
    c = hist.pop_board();
    ASSERT_EQ(c.get_halfmove_clock(), 0);

    c = c.make_move(Move::NULL_MOVE(), hist);
    c = c.make_move(Move::NULL_MOVE(), hist);
    c = c.make_move(Move::NULL_MOVE(), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 3);
    c = c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 35, 51), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c = hist.pop_board();
    ASSERT_EQ(c.get_halfmove_clock(), 3);
    c = c.make_move(Move(MoveFlags::QUIET_MOVE, 43, 51), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c = hist.pop_board();
    ASSERT_EQ(c.get_halfmove_clock(), 3);
    // Both pawn moves reset it

    c.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 24 48");
    hist = BoardHistory(c);
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    c = c.make_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 0, 9), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c = hist.pop_board();
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    c = c.make_move(Move(MoveFlags::BISHOP_PROMOTION, 1, 9), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c = hist.pop_board();
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    // Test promotions reset clock

    c.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 24 48");
    hist = BoardHistory(c);
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    const auto original_score = c.get_score(Side::WHITE);
    c = c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), hist);
    c = c.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c = hist.pop_board();
    c = hist.pop_board();
    ASSERT_EQ(original_score, c.get_score(Side::WHITE));
    ASSERT_EQ(c.get_halfmove_clock(), 24);
    // en passant captures are correctly handled

    c.set_from_fen("2r5/6k1/8/6K1/8/8/2Q5/8 w - - 12 24");
    hist = BoardHistory(c);
    ASSERT_EQ(c.get_halfmove_clock(), 12);
    c = c.make_move(Move(MoveFlags::CAPTURE, 58, 10), hist);
    ASSERT_EQ(c.get_halfmove_clock(), 0);
    c = hist.pop_board();
    ASSERT_EQ(c.get_halfmove_clock(), 12);
    // and tests normal captures reset it
}

TEST(ChessBoardTests, TestUnmakeScores) {
    ChessBoard c;
    c.set_from_fen("1r4k1/P7/8/3Pp3/8/1b6/P7/R3K2R w KQ e6 0 1");
    BoardHistory hist(c);
    const auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    const auto mg_score = c.get_score(Side::WHITE);
    for (size_t i = 0; i < moves.size(); i++) {
        c.make_move(moves[i].move, hist);
        hist.pop_board();
        ASSERT_EQ(c.get_score(Side::WHITE), mg_score) << "Score mismatch on move " << moves[i].move.to_string();
    }
}