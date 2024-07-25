#include <gtest/gtest.h>

#include <cstdint>

#include "../src/chessboard.hpp"
#include "../src/evaluation.hpp"
#include "../src/move_generator.hpp"

TEST(ChessBoardTests, SetFenStartPos) {
    Position pos;
    ASSERT_TRUE(pos.set_from_fen("startpos").has_value());

    ASSERT_EQ(pos.occupancy(), Bitboard(0xFFFF00000000FFFF));
    ASSERT_EQ(pos.pawns(), Bitboard(0x00FF00000000FF00));
    ASSERT_EQ(pos.rooks(), Bitboard(0x8100000000000081));
    ASSERT_EQ(pos.knights(), Bitboard(0x4200000000000042));
    ASSERT_EQ(pos.bishops(), Bitboard(0x2400000000000024));
    ASSERT_EQ(pos.queens(), Bitboard(0x0800000000000008));
    ASSERT_EQ(pos.kings(), Bitboard(0x1000000000000010));

    ASSERT_EQ(pos.occupancy(Side::WHITE), Bitboard(0x000000000000FFFF));
    ASSERT_EQ(pos.pawns(Side::WHITE), Bitboard(0x000000000000FF00));
    ASSERT_EQ(pos.rooks(Side::WHITE), Bitboard(0x0000000000000081));
    ASSERT_EQ(pos.knights(Side::WHITE), Bitboard(0x0000000000000042));
    ASSERT_EQ(pos.bishops(Side::WHITE), Bitboard(0x0000000000000024));
    ASSERT_EQ(pos.queens(Side::WHITE), Bitboard(0x0000000000000008));
    ASSERT_EQ(pos.kings(Side::WHITE), Bitboard(0x0000000000000010));

    ASSERT_EQ(pos.occupancy(Side::BLACK), Bitboard(0xFFFF000000000000));
    ASSERT_EQ(pos.pawns(Side::BLACK), Bitboard(0x00FF000000000000));
    ASSERT_EQ(pos.rooks(Side::BLACK), Bitboard(0x8100000000000000));
    ASSERT_EQ(pos.knights(Side::BLACK), Bitboard(0x4200000000000000));
    ASSERT_EQ(pos.bishops(Side::BLACK), Bitboard(0x2400000000000000));
    ASSERT_EQ(pos.queens(Side::BLACK), Bitboard(0x0800000000000000));
    ASSERT_EQ(pos.kings(Side::BLACK), Bitboard(0x1000000000000000));

    ASSERT_EQ(pos.stm(), Side::WHITE);
    ASSERT_EQ(pos.get_en_passant_file(), 9);
}

TEST(ChessBoardTests, SetFenExplicitStartPos) {
    Position pos;
    ASSERT_TRUE(pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    ASSERT_EQ(pos.occupancy(), Bitboard(0xFFFF00000000FFFF));
    ASSERT_EQ(pos.pawns(), Bitboard(0x00FF00000000FF00));
    ASSERT_EQ(pos.rooks(), Bitboard(0x8100000000000081));
    ASSERT_EQ(pos.knights(), Bitboard(0x4200000000000042));
    ASSERT_EQ(pos.bishops(), Bitboard(0x2400000000000024));
    ASSERT_EQ(pos.queens(), Bitboard(0x0800000000000008));
    ASSERT_EQ(pos.kings(), Bitboard(0x1000000000000010));

    ASSERT_EQ(pos.occupancy(Side::WHITE), Bitboard(0x000000000000FFFF));
    ASSERT_EQ(pos.pawns(Side::WHITE), Bitboard(0x000000000000FF00));
    ASSERT_EQ(pos.rooks(Side::WHITE), Bitboard(0x0000000000000081));
    ASSERT_EQ(pos.knights(Side::WHITE), Bitboard(0x0000000000000042));
    ASSERT_EQ(pos.bishops(Side::WHITE), Bitboard(0x0000000000000024));
    ASSERT_EQ(pos.queens(Side::WHITE), Bitboard(0x0000000000000008));
    ASSERT_EQ(pos.kings(Side::WHITE), Bitboard(0x0000000000000010));

    ASSERT_EQ(pos.occupancy(Side::BLACK), Bitboard(0xFFFF000000000000));
    ASSERT_EQ(pos.pawns(Side::BLACK), Bitboard(0x00FF000000000000));
    ASSERT_EQ(pos.rooks(Side::BLACK), Bitboard(0x8100000000000000));
    ASSERT_EQ(pos.knights(Side::BLACK), Bitboard(0x4200000000000000));
    ASSERT_EQ(pos.bishops(Side::BLACK), Bitboard(0x2400000000000000));
    ASSERT_EQ(pos.queens(Side::BLACK), Bitboard(0x0800000000000000));
    ASSERT_EQ(pos.kings(Side::BLACK), Bitboard(0x1000000000000000));

    ASSERT_EQ(pos.stm(), Side::WHITE);
    ASSERT_EQ(pos.get_en_passant_file(), 9);
}

TEST(ChessBoardTests, SetFenExplicitStartPosShort) {
    Position pos;
    ASSERT_FALSE(pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR "));

    ASSERT_EQ(pos.occupancy(), Bitboard(0xFFFF00000000FFFF));
    ASSERT_EQ(pos.pawns(), Bitboard(0x00FF00000000FF00));
    ASSERT_EQ(pos.rooks(), Bitboard(0x8100000000000081));
    ASSERT_EQ(pos.knights(), Bitboard(0x4200000000000042));
    ASSERT_EQ(pos.bishops(), Bitboard(0x2400000000000024));
    ASSERT_EQ(pos.queens(), Bitboard(0x0800000000000008));
    ASSERT_EQ(pos.kings(), Bitboard(0x1000000000000010));

    ASSERT_EQ(pos.occupancy(Side::WHITE), Bitboard(0x000000000000FFFF));
    ASSERT_EQ(pos.pawns(Side::WHITE), Bitboard(0x000000000000FF00));
    ASSERT_EQ(pos.rooks(Side::WHITE), Bitboard(0x0000000000000081));
    ASSERT_EQ(pos.knights(Side::WHITE), Bitboard(0x0000000000000042));
    ASSERT_EQ(pos.bishops(Side::WHITE), Bitboard(0x0000000000000024));
    ASSERT_EQ(pos.queens(Side::WHITE), Bitboard(0x0000000000000008));
    ASSERT_EQ(pos.kings(Side::WHITE), Bitboard(0x0000000000000010));

    ASSERT_EQ(pos.occupancy(Side::BLACK), Bitboard(0xFFFF000000000000));
    ASSERT_EQ(pos.pawns(Side::BLACK), Bitboard(0x00FF000000000000));
    ASSERT_EQ(pos.rooks(Side::BLACK), Bitboard(0x8100000000000000));
    ASSERT_EQ(pos.knights(Side::BLACK), Bitboard(0x4200000000000000));
    ASSERT_EQ(pos.bishops(Side::BLACK), Bitboard(0x2400000000000000));
    ASSERT_EQ(pos.queens(Side::BLACK), Bitboard(0x0800000000000000));
    ASSERT_EQ(pos.kings(Side::BLACK), Bitboard(0x1000000000000000));
}

TEST(ChessBoardTests, TestMakeUnmakeMove) {
    Position pos, original;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    original.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    const auto original_score = pos.get_score(Side::WHITE);
    auto BishopMoves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);
    BoardHistory hist(pos);
    for (size_t j = 0; j < BishopMoves.size(); j++) {
        Move m = BishopMoves[j].move;
        ASSERT_EQ(hist.len(), 1);
        pos = pos.make_move(m, hist);
        ASSERT_EQ(hist.len(), 2);
        pos = hist.pop_board();
        ASSERT_EQ(original_score, pos.get_score(Side::WHITE)) << "Score mismatch after move " << m.to_string();
    for (Square sq = Square::A1; sq != Square::NONE; sq++) {
            ASSERT_EQ(pos.piece_at(sq).get_value(), original.piece_at(sq).get_value())
                << "Mismatch at square " << std::to_string(sq_to_int(sq)) << " after move " << m.to_string() << " with flags "
                << std::to_string(static_cast<int>(m.flags())) << " (value " << std::to_string(m.value()) << ")";
        }
    }

    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    original.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    hist = BoardHistory(pos);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8), hist);
    pos = hist.pop_board();
    for (Square sq = Square::A1; sq != Square::NONE; sq++) {
        ASSERT_EQ(pos.piece_at(sq).get_value(), original.piece_at(sq).get_value()) << "Mismatch at square " << std::to_string(sq_to_int(sq)) << "!";
    }

    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    original.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    hist = BoardHistory(pos);
    pos = pos.make_move(Move(6322), hist);
    pos = hist.pop_board();
    for (Square sq = Square::A1; sq != Square::NONE; sq++) {
        ASSERT_EQ(pos.piece_at(sq).get_value(), original.piece_at(sq).get_value()) << "Mismatch at square " << std::to_string(sq_to_int(sq)) << "!";
    }
}

TEST(ChessBoardTests, TestMakeMove) {
    Position pos, original;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    original.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1");
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), hist);
    ASSERT_STREQ(Move(MoveFlags::QUIET_MOVE, 17, 9).to_string().c_str(), "b2b3");
    for (Square sq = Square::A1; sq != Square::NONE; sq++) {
        ASSERT_EQ(pos.piece_at(sq).get_value(), original.piece_at(sq).get_value()) << "Mismatch at square " << std::to_string(sq_to_int(sq)) << "!";
    }
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    original.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    hist = BoardHistory(pos);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8), hist);
    ASSERT_STREQ(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8).to_string().c_str(), "a2a4");
    ASSERT_EQ(Move(MoveFlags::DOUBLE_PAWN_PUSH, 24, 8).value(), 5640);
    for (Square sq = Square::A1; sq != Square::NONE; sq++) {
        ASSERT_EQ(pos.piece_at(sq).get_value(), original.piece_at(sq).get_value()) << "Mismatch at square " << std::to_string(sq_to_int(sq)) << "!";
    }
}

TEST(ChessBoardTests, TestMakePromotion) {
    Position pos, original;
    pos.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1");
    original.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/P2P2PP/b2Q1R1K w kq - 0 2");
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 0, 9), hist);
    ASSERT_EQ(pos, original);
}

TEST(ChessBoardTests, TestEquality) {
    Position pos, original;
    pos.set_from_fen("startpos");
    original.set_from_fen("startpos");
    ASSERT_EQ(pos, original);
}

TEST(ChessBoardTests, TestClearBoard) {
    Position pos;
    pos.set_from_fen("startpos");
    pos.clear_board();
    for (int i = 0; i < 12; i++) {
        ASSERT_TRUE(pos.get_bb(i / 2, i % 2).empty());
    }
    ASSERT_EQ(pos.stm(), Side::WHITE);
    ASSERT_EQ(pos.get_en_passant_file(), 9);
    ASSERT_FALSE(pos.get_kingside_castling(Side::WHITE));
    ASSERT_FALSE(pos.get_kingside_castling(Side::BLACK));
    ASSERT_FALSE(pos.get_queenside_castling(Side::WHITE));
    ASSERT_FALSE(pos.get_queenside_castling(Side::BLACK));
}

TEST(ChessBoardTests, TestPrintBoard) {
    Position pos;
    pos.set_from_fen("startpos");
    testing::internal::CaptureStdout();
    pos.print_board();
    std::string from_stdout = testing::internal::GetCapturedStdout();
    ASSERT_STREQ("rnbqkbnr\npppppppp\n........\n........\n........\n........\nPPPPPPPP\nRNBQKBNR\n", from_stdout.c_str());
}

TEST(ChessBoardTests, TestPromotionsFromString) {
    Position pos;
    pos.set_from_fen("1r6/P7/5k2/8/5K2/8/8/8 w - - 0 1");
    ASSERT_EQ(pos.generate_move_from_string("a7a8r").value(), Move(MoveFlags::ROOK_PROMOTION, 56, 48));
    ASSERT_EQ(pos.generate_move_from_string("a7a8n").value(), Move(MoveFlags::KNIGHT_PROMOTION, 56, 48));
    ASSERT_EQ(pos.generate_move_from_string("a7a8b").value(), Move(MoveFlags::BISHOP_PROMOTION, 56, 48));
    ASSERT_EQ(pos.generate_move_from_string("a7a8q").value(), Move(MoveFlags::QUEEN_PROMOTION, 56, 48));

    ASSERT_EQ(pos.generate_move_from_string("a7b8r").value(), Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48));
    ASSERT_EQ(pos.generate_move_from_string("a7b8n").value(), Move(MoveFlags::KNIGHT_PROMOTION_CAPTURE, 57, 48));
    ASSERT_EQ(pos.generate_move_from_string("a7b8b").value(), Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 57, 48));
    ASSERT_EQ(pos.generate_move_from_string("a7b8q").value(), Move(MoveFlags::QUEEN_PROMOTION_CAPTURE, 57, 48));
}

TEST(ChessBoardTests, TestInvalidMoveString) {
    Position pos;
    pos.set_from_fen("startpos");
    ASSERT_FALSE(pos.generate_move_from_string("dfjkgsjkfdj").has_value());
    ASSERT_FALSE(pos.generate_move_from_string("a7a8c").has_value());
}

TEST(ChessBoardTests, TestEnPassantFromString) {
    Position pos;
    pos.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    ASSERT_EQ(pos.generate_move_from_string("d4e3"), Move(MoveFlags::EN_PASSANT_CAPTURE, 20, 27));
    ASSERT_EQ(pos.generate_move_from_string("c2c4"), Move(MoveFlags::DOUBLE_PAWN_PUSH, 26, 10));
}

TEST(ChessBoardTests, TestCastlingFromString) {
    Position pos;
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    ASSERT_EQ(pos.generate_move_from_string("e1c1"), Move(MoveFlags::QUEENSIDE_CASTLE, 2, 4));
    ASSERT_EQ(pos.generate_move_from_string("e1g1"), Move(MoveFlags::KINGSIDE_CASTLE, 6, 4));
    ASSERT_EQ(pos.generate_move_from_string("e1f1"), Move(MoveFlags::QUIET_MOVE, 5, 4));
    // not a castling move but included just to cover this branch
}

TEST(ChessBoardTests, TestMakeNullMove) {
    Position pos, original;
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    original.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 2");
    BoardHistory hist(pos);
    pos = pos.make_move(0, hist);
    ASSERT_EQ(pos, original);

    pos.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    original.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    hist = BoardHistory(pos);
    pos = pos.make_move(0, hist);
    ASSERT_EQ(pos, original);

    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    original.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR b Kkq - 0 2");
    hist = BoardHistory(pos);
    pos = pos.make_move(0, hist);
    ASSERT_EQ(pos, original);
}

TEST(ChessBoardTests, TestUnmakeNullMove) {
    Position pos, original;
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    original.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    BoardHistory hist(pos);
    pos = pos.make_move(0, hist);
    pos = hist.pop_board();
    ASSERT_EQ(pos, original);

    pos.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    original.set_from_fen("rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    hist = BoardHistory(pos);
    pos = pos.make_move(0, hist);
    pos = hist.pop_board();
    ASSERT_EQ(pos, original);

    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    original.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    hist = BoardHistory(pos);
    pos = pos.make_move(0, hist);
    pos = hist.pop_board();
    ASSERT_EQ(pos, original);
}

TEST(ChessBoardTests, TestHalfmoveClock) {
    Position pos;
    pos.set_from_fen("startpos");
    ASSERT_EQ(pos.get_halfmove_clock(), 0);

    BoardHistory hist(pos);
    pos = pos.make_move(Move::NULL_MOVE(), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 1);
    pos = hist.pop_board();
    ASSERT_EQ(pos.get_halfmove_clock(), 0);

    pos = pos.make_move(Move::NULL_MOVE(), hist);
    pos = pos.make_move(Move::NULL_MOVE(), hist);
    pos = pos.make_move(Move::NULL_MOVE(), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 3);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 35, 51), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 0);
    pos = hist.pop_board();
    ASSERT_EQ(pos.get_halfmove_clock(), 3);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 43, 51), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 0);
    pos = hist.pop_board();
    ASSERT_EQ(pos.get_halfmove_clock(), 3);
    // Both pawn moves reset it

    pos.set_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 24 48");
    hist = BoardHistory(pos);
    ASSERT_EQ(pos.get_halfmove_clock(), 24);
    pos = pos.make_move(Move(MoveFlags::BISHOP_PROMOTION_CAPTURE, 0, 9), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 0);
    pos = hist.pop_board();
    ASSERT_EQ(pos.get_halfmove_clock(), 24);
    pos = pos.make_move(Move(MoveFlags::BISHOP_PROMOTION, 1, 9), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 0);
    pos = hist.pop_board();
    ASSERT_EQ(pos.get_halfmove_clock(), 24);
    // Test promotions reset clock

    pos.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 24 48");
    hist = BoardHistory(pos);
    ASSERT_EQ(pos.get_halfmove_clock(), 24);
    const auto original_score = pos.get_score(Side::WHITE);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), hist);
    pos = pos.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 0);
    pos = hist.pop_board();
    pos = hist.pop_board();
    ASSERT_EQ(original_score, pos.get_score(Side::WHITE));
    ASSERT_EQ(pos.get_halfmove_clock(), 24);
    // en passant captures are correctly handled

    pos.set_from_fen("2r5/6k1/8/6K1/8/8/2Q5/8 w - - 12 24");
    hist = BoardHistory(pos);
    ASSERT_EQ(pos.get_halfmove_clock(), 12);
    pos = pos.make_move(Move(MoveFlags::CAPTURE, 58, 10), hist);
    ASSERT_EQ(pos.get_halfmove_clock(), 0);
    pos = hist.pop_board();
    ASSERT_EQ(pos.get_halfmove_clock(), 12);
    // and tests normal captures reset it
}

TEST(ChessBoardTests, TestUnmakeScores) {
    Position pos;
    pos.set_from_fen("1r4k1/P7/8/3Pp3/8/1b6/P7/R3K2R w KQ e6 0 1");
    BoardHistory hist(pos);
    const auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);
    const auto mg_score = pos.get_score(Side::WHITE);
    for (size_t i = 0; i < moves.size(); i++) {
        pos.make_move(moves[i].move, hist);
        hist.pop_board();
        ASSERT_EQ(pos.get_score(Side::WHITE), mg_score) << "Score mismatch on move " << moves[i].move.to_string();
    }
}