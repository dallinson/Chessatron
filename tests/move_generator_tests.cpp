#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/magic_numbers.hpp"
#include "../src/move_generator.hpp"

TEST(MoveGeneratorTests, TestCorrectMoveCountStartPos) {
    ChessBoard c;
    c.set_from_fen("startpos");

    auto lst = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    ASSERT_EQ(lst.len(), 20);
    MoveList m;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 16);
    m = MoveList();
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 4);

    lst = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK);
    ASSERT_EQ(lst.len(), 20);
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 16);
    m = MoveList();
    m = MoveList();
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 4);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountMaxMoves) {
    ChessBoard c;
    c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(c, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.len(), 19);

    auto lst = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    ASSERT_EQ(lst.len(), 218);
}

TEST(MoveGeneratorTests, TestEnPassant) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR ");
    c.set_en_passant_file(4);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, pawn_moves);
    ASSERT_EQ(pawn_moves.len(), 16);
}

TEST(MoveGeneratorTests, TestDoubleCheck) {
    ChessBoard c;
    c.set_from_fen("7k/8/b3r3/8/8/8/4K3/1Q6 w - - 0 1");
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    ASSERT_EQ(moves.len(), 4);
    auto king_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);
    ASSERT_EQ(king_moves.len(), moves.len());
}

TEST(MoveGeneratorTests, TestPawnPromotions) {
    ChessBoard c;
    MoveList m;
    c.set_from_fen("8/2P2k2/8/8/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 4);
    c.set_from_fen("8/5k2/8/8/8/8/2p2K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 4);
}

TEST(MoveGeneratorTests, TestPawnCapturePromotions) {
    ChessBoard c;
    MoveList m;

    c.set_from_fen("6r1/5k1P/8/8/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 8);
    c.set_from_fen("1r6/P4k2/8/8/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 8);

    c.set_from_fen("8/5k2/8/8/8/8/p4K2/1R6 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 8);
    c.set_from_fen("8/5k2/8/8/8/8/5K1p/6R1 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 8);
}

TEST(MoveGeneratorTests, TestPawnCaptures) {
    ChessBoard c;
    MoveList m;

    c.set_from_fen("8/5k2/1r1r4/2P5/8/8/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 3);

    c.set_from_fen("8/5k2/8/8/2p5/1R1R4/5K2/8 w - - 0 1");
    m = MoveList();
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 3);
}

TEST(MoveGeneratorTests, TestCastling) {
    ChessBoard c;
    MoveList m;

    c.set_kingside_castling(Side::WHITE, true);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 1);
    // ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::WHITE)[0].get_move(), 8454);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKn1R w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 0);

    c.set_queenside_castling(Side::WHITE, true);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 1);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1n1KBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::WHITE, m);
    ASSERT_EQ(m.len(), 0);

    c.set_kingside_castling(Side::BLACK, true);
    c.set_from_fen("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 1);
    c.set_from_fen("rnbqkN1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 0);

    c.set_queenside_castling(Side::WHITE, true);
    c.set_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 1);
    c.set_from_fen("r1N1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    m = MoveList();
    MoveGenerator::generate_castling_moves(c, Side::BLACK, m);
    ASSERT_EQ(m.len(), 0);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipete) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    auto legal_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);

    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(c, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.len(), 5);
    ASSERT_EQ(rook_moves.len(), 5);

    MoveList bishop_moves;
    MoveGenerator::generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(c, Side::WHITE, bishop_moves);
    ASSERT_EQ(bishop_moves.len(), 11);

    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(c, Side::WHITE, king_moves);
    ASSERT_EQ(king_moves.len(), 2);

    MoveList castling_moves;
    MoveGenerator::generate_castling_moves(c, Side::WHITE, castling_moves);
    ASSERT_EQ(castling_moves.len(), 2);

    MoveList queen_moves;
    MoveGenerator::generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(c, Side::WHITE, queen_moves);
    ASSERT_EQ(queen_moves.len(), 9);

    MoveList knight_moves;
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(c, Side::WHITE, knight_moves);
    ASSERT_EQ(knight_moves.len(), 11);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, pawn_moves);
    ASSERT_EQ(pawn_moves.len(), 8);

    ASSERT_EQ(legal_moves.len(), 48);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipeteB2B3) {
    ChessBoard c;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1");
    auto legal_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK);

    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(c, Side::BLACK, rook_moves);
    ASSERT_EQ(rook_moves.len(), 9);

    MoveList bishop_moves;
    MoveGenerator::generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(c, Side::BLACK, bishop_moves);
    ASSERT_EQ(bishop_moves.len(), 8);

    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(c, Side::BLACK, king_moves);
    ASSERT_EQ(king_moves.len(), 2);

    MoveList castling_moves;
    MoveGenerator::generate_castling_moves(c, Side::BLACK, castling_moves);
    ASSERT_EQ(castling_moves.len(), 2);

    MoveList queen_moves;
    MoveGenerator::generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(c, Side::BLACK, queen_moves);
    ASSERT_EQ(queen_moves.len(), 4);

    MoveList knight_moves;
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(c, Side::BLACK, knight_moves);
    ASSERT_EQ(knight_moves.len(), 10);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::BLACK, pawn_moves);
    ASSERT_EQ(pawn_moves.len(), 7);

    ASSERT_EQ(legal_moves.len(), 42);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipeteE1D1C7C6) {
    ChessBoard c;
    BoardHistory history;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    c = c.make_move(Move(2738), history);

    auto legal_moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE);

    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(c, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.len(), 5);

    MoveList bishop_moves;
    MoveGenerator::generate_moves<PieceTypes::BISHOP, MoveGenType::ALL_LEGAL>(c, Side::WHITE, bishop_moves);
    ASSERT_EQ(bishop_moves.len(), 11);

    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(c, Side::WHITE, king_moves);
    ASSERT_EQ(king_moves.len(), 2);

    MoveList castling_moves;
    MoveGenerator::generate_castling_moves(c, Side::WHITE, castling_moves);
    ASSERT_EQ(castling_moves.len(), 0);

    MoveList queen_moves;
    MoveGenerator::generate_moves<PieceTypes::QUEEN, MoveGenType::ALL_LEGAL>(c, Side::WHITE, queen_moves);
    ASSERT_EQ(queen_moves.len(), 9);

    MoveList knight_moves;
    MoveGenerator::generate_moves<PieceTypes::KNIGHT, MoveGenType::ALL_LEGAL>(c, Side::WHITE, knight_moves);
    ASSERT_EQ(knight_moves.len(), 10);

    MoveList pawn_moves;
    MoveGenerator::generate_pawn_moves<MoveGenType::ALL_LEGAL>(c, Side::WHITE, pawn_moves);
    ASSERT_EQ(pawn_moves.len(), 9);

    ASSERT_EQ(legal_moves.len(), 46);
}

TEST(MoveGeneratorTests, TestRookMoves) {
    ChessBoard c;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    MoveList rook_moves;
    MoveGenerator::generate_moves<PieceTypes::ROOK, MoveGenType::ALL_LEGAL>(c, Side::WHITE, rook_moves);
    ASSERT_EQ(rook_moves.len(), 5);
}

TEST(MoveGeneratorTests, TestWhiteBishopKing) {
    ChessBoard c;
    c.set_from_fen("1B6/8/2r5/1knK4/8/8/8/8 w - - 16 166");
    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(c, Side::WHITE, king_moves);
    ASSERT_EQ(king_moves.len(), 2);
}

TEST(MoveGeneratorTests, TestMoveKingNearKing) {
    ChessBoard c;
    c.set_from_fen("5n2/8/P4P1B/1p6/1p1P2P1/1k6/8/K7 b - - 0 72");
    MoveList king_moves;
    MoveGenerator::generate_moves<PieceTypes::KING, MoveGenType::ALL_LEGAL>(c, Side::BLACK, king_moves);
    ASSERT_EQ(king_moves.len(), 5);
}

TEST(MoveGeneratorTests, TestCorrectCaptureCount) {
    ChessBoard c;
    c.set_from_fen("6kr/pp2r2p/n1p1PB1Q/2q5/2B4P/2N3p1/PPP3P1/7K w - - 1 0");
    // Taken from https://wtharvey.com/m8n4.txt, Serafino Dubois vs Augustus Mongredien, London, 1862
    auto moves = MoveGenerator::generate_legal_moves<MoveGenType::ALL_LEGAL>(c, c.get_side_to_move());
    int captures = 0;
    for (size_t i = 0; i < moves.len(); i++) {
        if (moves[i].move.is_capture()) {
            captures += 1;
        }
    }
    ASSERT_EQ(captures, 4);
}

TEST(MoveGeneratorTests, TestThreatenedCastle) {
    ChessBoard c;
    c.set_from_fen("3r4/8/2r4p/p2n2p1/6P1/3p3P/P2B2k1/3RK2R w K - 1 39");
    MoveList moves;
    MoveGenerator::generate_castling_moves(c, c.get_side_to_move(), moves);
    ASSERT_EQ(moves.len(), 0);
}