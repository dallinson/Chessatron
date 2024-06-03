#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/magic_numbers.hpp"
#include "../src/move_generator.hpp"

TEST(MoveGeneratorTests, TestCorrectMoveCountStartPos) {
    Position pos;
    pos.set_from_fen("startpos");

    auto lst = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);
    ASSERT_EQ(lst.size(), 20);
    MoveList m;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, m);
    ASSERT_EQ(m.size(), 16);
    m = MoveList();
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, m);
    ASSERT_EQ(m.size(), 4);

    lst = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::BLACK);
    ASSERT_EQ(lst.size(), 20);
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, m);
    ASSERT_EQ(m.size(), 16);
    m = MoveList();
    m = MoveList();
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, m);
    ASSERT_EQ(m.size(), 4);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountMaxMoves) {
    Position pos;
    pos.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.size(), 19);

    auto lst = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);
    ASSERT_EQ(lst.size(), 218);
}

TEST(MoveGeneratorTests, TestEnPassant) {
    Position pos;
    pos.set_from_fen("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR ");
    pos.set_en_passant_file(4);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, pawn_moves);
    ASSERT_EQ(pawn_moves.size(), 16);
}

TEST(MoveGeneratorTests, TestDoubleCheck) {
    Position pos;
    pos.set_from_fen("7k/8/b3r3/8/8/8/4K3/1Q6 w - - 0 1");
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);
    ASSERT_EQ(moves.size(), 4);
    auto king_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);
    ASSERT_EQ(king_moves.size(), moves.size());
}

TEST(MoveGeneratorTests, TestPawnPromotions) {
    Position pos;
    MoveList m;
    pos.set_from_fen("8/2P2k2/8/8/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, m);
    ASSERT_EQ(m.size(), 4);
    pos.set_from_fen("8/5k2/8/8/8/8/2p2K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, m);
    ASSERT_EQ(m.size(), 4);
}

TEST(MoveGeneratorTests, TestPawnCapturePromotions) {
    Position pos;
    MoveList m;

    pos.set_from_fen("6r1/5k1P/8/8/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, m);
    ASSERT_EQ(m.size(), 8);
    pos.set_from_fen("1r6/P4k2/8/8/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, m);
    ASSERT_EQ(m.size(), 8);

    pos.set_from_fen("8/5k2/8/8/8/8/p4K2/1R6 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, m);
    ASSERT_EQ(m.size(), 8);
    pos.set_from_fen("8/5k2/8/8/8/8/5K1p/6R1 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, m);
    ASSERT_EQ(m.size(), 8);
}

TEST(MoveGeneratorTests, TestPawnCaptures) {
    Position pos;
    MoveList m;

    pos.set_from_fen("8/5k2/1r1r4/2P5/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, m);
    ASSERT_EQ(m.size(), 3);

    pos.set_from_fen("8/5k2/8/8/2p5/1R1R4/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, m);
    ASSERT_EQ(m.size(), 3);
}

TEST(MoveGeneratorTests, TestCastling) {
    Position pos;
    MoveList m;

    pos.set_kingside_castling(Side::WHITE, true);
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::WHITE, m);
    ASSERT_EQ(m.size(), 1);
    // ASSERT_EQ(MoveGenerator::generate_castling_moves(pos, Side::WHITE)[0].get_move(), 8454);
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKn1R w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::WHITE, m);
    ASSERT_EQ(m.size(), 0);

    pos.set_queenside_castling(Side::WHITE, true);
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::WHITE, m);
    ASSERT_EQ(m.size(), 1);
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1n1KBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::WHITE, m);
    ASSERT_EQ(m.size(), 0);

    pos.set_kingside_castling(Side::BLACK, true);
    pos.set_from_fen("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::BLACK, m);
    ASSERT_EQ(m.size(), 1);
    pos.set_from_fen("rnbqkN1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::BLACK, m);
    ASSERT_EQ(m.size(), 0);

    pos.set_queenside_castling(Side::WHITE, true);
    pos.set_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::BLACK, m);
    ASSERT_EQ(m.size(), 1);
    pos.set_from_fen("r1N1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(pos, Side::BLACK, m);
    ASSERT_EQ(m.size(), 0);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipete) {
    Position pos, origin;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    origin.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    auto legal_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);

    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.size(), 5);
    ASSERT_EQ(rook_moves.size(), 5);

    MoveList bishop_moves;
    MoveGenerator::generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, bishop_moves);
    ASSERT_EQ(bishop_moves.size(), 11);

    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, king_moves);
    ASSERT_EQ(king_moves.size(), 2);

    MoveList castling_moves;
    MoveGenerator::generate_castling_moves(pos, Side::WHITE, castling_moves);
    ASSERT_EQ(castling_moves.size(), 2);

    MoveList queen_moves;
    MoveGenerator::generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, queen_moves);
    ASSERT_EQ(queen_moves.size(), 9);

    MoveList knight_moves;
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, knight_moves);
    ASSERT_EQ(knight_moves.size(), 11);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, pawn_moves);
    ASSERT_EQ(pawn_moves.size(), 8);

    ASSERT_EQ(legal_moves.size(), 48);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipeteB2B3) {
    Position pos;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1");
    auto legal_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::BLACK);

    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, rook_moves);
    ASSERT_EQ(rook_moves.size(), 9);

    MoveList bishop_moves;
    MoveGenerator::generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, bishop_moves);
    ASSERT_EQ(bishop_moves.size(), 8);

    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, king_moves);
    ASSERT_EQ(king_moves.size(), 2);

    MoveList castling_moves;
    MoveGenerator::generate_castling_moves(pos, Side::BLACK, castling_moves);
    ASSERT_EQ(castling_moves.size(), 2);

    MoveList queen_moves;
    MoveGenerator::generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, queen_moves);
    ASSERT_EQ(queen_moves.size(), 4);

    MoveList knight_moves;
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, knight_moves);
    ASSERT_EQ(knight_moves.size(), 10);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::BLACK>(pos, pawn_moves);
    ASSERT_EQ(pawn_moves.size(), 7);

    ASSERT_EQ(legal_moves.size(), 42);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipeteE1D1C7C6) {
    Position pos;
    BoardHistory history;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    pos = pos.make_move(Move(2738), history);

    auto legal_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, Side::WHITE);

    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.size(), 5);

    MoveList bishop_moves;
    MoveGenerator::generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, bishop_moves);
    ASSERT_EQ(bishop_moves.size(), 11);

    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, king_moves);
    ASSERT_EQ(king_moves.size(), 2);

    MoveList castling_moves;
    MoveGenerator::generate_castling_moves(pos, Side::WHITE, castling_moves);
    ASSERT_EQ(castling_moves.size(), 0);

    MoveList queen_moves;
    MoveGenerator::generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, queen_moves);
    ASSERT_EQ(queen_moves.size(), 9);

    MoveList knight_moves;
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, knight_moves);
    ASSERT_EQ(knight_moves.size(), 10);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL, Side::WHITE>(pos, pawn_moves);
    ASSERT_EQ(pawn_moves.size(), 9);

    ASSERT_EQ(legal_moves.size(), 46);
}

TEST(MoveGeneratorTests, TestRookMoves) {
    Position pos;
    pos.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.size(), 5);
}

TEST(MoveGeneratorTests, TestWhiteBishopKing) {
    Position pos;
    pos.set_from_fen("1B6/8/2r5/1knK4/8/8/8/8 w - - 16 166");
    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(pos, Side::WHITE, king_moves);
    ASSERT_EQ(king_moves.size(), 2);
}

TEST(MoveGeneratorTests, TestMoveKingNearKing) {
    Position pos;
    pos.set_from_fen("5n2/8/P4P1B/1p6/1p1P2P1/1k6/8/K7 b - - 0 72");
    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(pos, Side::BLACK, king_moves);
    ASSERT_EQ(king_moves.size(), 5);
}

TEST(MoveGeneratorTests, TestCorrectCaptureCount) {
    Position pos;
    pos.set_from_fen("6kr/pp2r2p/n1p1PB1Q/2q5/2B4P/2N3p1/PPP3P1/7K w - - 1 0");
    // Taken from https://wtharvey.com/m8n4.txt, Serafino Dubois vs Augustus Mongredien, London, 1862
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(pos, pos.stm());
    int captures = 0;
    for (size_t i = 0; i < moves.size(); i++) {
        if (moves[i].move.is_capture()) {
            captures += 1;
        }
    }
    ASSERT_EQ(captures, 4);
}

TEST(MoveGeneratorTests, TestThreatenedCastle) {
    Position pos;
    pos.set_from_fen("3r4/8/2r4p/p2n2p1/6P1/3p3P/P2B2k1/3RK2R w K - 1 39");
    MoveList moves;
    MoveGenerator::generate_castling_moves(pos, pos.stm(), moves);
    ASSERT_EQ(moves.size(), 0);
}