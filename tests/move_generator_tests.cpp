#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/move_generator.hpp"

TEST(MoveGeneratorTests, TestCorrectMoveCountStartPos) {
    ChessBoard c;
    c.set_from_fen("startpos");

    auto lst = MoveGenerator::generate_moves(c, 0);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, 0).len(), 4);

    lst = MoveGenerator::generate_moves(c, 1);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 1).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, 1).len(), 4);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountMaxMoves) {
    ChessBoard c;
    c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    auto rook_moves = MoveGenerator::generate_rook_moves(c, 0);
    ASSERT_EQ(rook_moves.len(), 19);

    auto lst = MoveGenerator::generate_moves(c, 0);
    ASSERT_EQ(lst.len(), 218);
}

TEST(MoveGeneratorTests, TestEnPassant) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR ");
    c.set_en_passant_file(4);

    auto pawn_moves = MoveGenerator::generate_pawn_moves(c, 0);
    ASSERT_EQ(pawn_moves.len(), 16);
}

TEST(MoveGeneratorTests, TestDoubleCheck) {
    ChessBoard c;
    c.set_from_fen("7k/8/b3r3/8/8/8/4K3/1Q6 w - - 0 1");
    auto moves = MoveGenerator::generate_moves(c, 0);
    ASSERT_EQ(moves.len(), 4);
    auto king_moves = MoveGenerator::generate_king_moves(c, 0);
    ASSERT_EQ(king_moves.len(), moves.len());
}

TEST(MoveGeneratorTests, TestPawnPromotions) {
    ChessBoard c;
    c.set_from_fen("8/2P2k2/8/8/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 4);
    c.set_from_fen("8/5k2/8/8/8/8/2p2K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 1).len(), 4);
}

TEST(MoveGeneratorTests, TestPawnCapturePromotions) {
    ChessBoard c;

    c.set_from_fen("6r1/5k1P/8/8/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 8);
    c.set_from_fen("1r6/P4k2/8/8/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 8);

    c.set_from_fen("8/5k2/8/8/8/8/p4K2/1R6 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 1).len(), 8);
    c.set_from_fen("8/5k2/8/8/8/8/5K1p/6R1 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 1).len(), 8);
}

TEST(MoveGeneratorTests, TestPawnCaptures) {
    ChessBoard c;

    c.set_from_fen("8/5k2/1r1r4/2P5/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 0).len(), 3);

    c.set_from_fen("8/5k2/8/8/2p5/1R1R4/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, 1).len(), 3);
}

TEST(MoveGeneratorTests, TestCastling) {
    ChessBoard c;
    c.set_kingside_castling(0, true);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 0).len(), 1);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKn1R w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 0).len(), 0);

    c.set_queenside_castling(0, true);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 0).len(), 1);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1n1KBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 0).len(), 0);

    c.set_kingside_castling(1, true);
    c.set_from_fen("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 1).len(), 1);
    c.set_from_fen("rnbqkN1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 1).len(), 0);

    c.set_queenside_castling(0, true);
    c.set_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 1).len(), 1);
    c.set_from_fen("r1N1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, 1).len(), 0);
}