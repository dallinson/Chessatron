#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/magic_numbers.hpp"
#include "../src/move_generator.hpp"

TEST(MoveGeneratorTests, TestCorrectMoveCountStartPos) {
    ChessBoard c;
    c.set_from_fen("startpos");

    auto lst = MoveGenerator::generate_legal_moves(c, Side::WHITE);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::WHITE).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, Side::WHITE).len(), 4);

    lst = MoveGenerator::generate_legal_moves(c, Side::BLACK);
    ASSERT_EQ(lst.len(), 20);
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::BLACK).len(), 16);
    ASSERT_EQ(MoveGenerator::generate_knight_moves(c, Side::BLACK).len(), 4);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountMaxMoves) {
    ChessBoard c;
    c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    auto rook_moves = MoveGenerator::generate_rook_moves(c, Side::WHITE);
    ASSERT_EQ(rook_moves.len(), 19);

    auto lst = MoveGenerator::generate_legal_moves(c, Side::WHITE);
    ASSERT_EQ(lst.len(), 218);
}

TEST(MoveGeneratorTests, TestEnPassant) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR ");
    c.set_en_passant_file(4);

    auto pawn_moves = MoveGenerator::generate_pawn_moves(c, Side::WHITE);
    ASSERT_EQ(pawn_moves.len(), 16);
}

TEST(MoveGeneratorTests, TestDoubleCheck) {
    ChessBoard c;
    c.set_from_fen("7k/8/b3r3/8/8/8/4K3/1Q6 w - - 0 1");
    auto moves = MoveGenerator::generate_legal_moves(c, Side::WHITE);
    ASSERT_EQ(moves.len(), 4);
    auto king_moves = MoveGenerator::generate_legal_moves(c, Side::WHITE);
    ASSERT_EQ(king_moves.len(), moves.len());
}

TEST(MoveGeneratorTests, TestPawnPromotions) {
    ChessBoard c;
    c.set_from_fen("8/2P2k2/8/8/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::WHITE).len(), 4);
    c.set_from_fen("8/5k2/8/8/8/8/2p2K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::BLACK).len(), 4);
}

TEST(MoveGeneratorTests, TestPawnCapturePromotions) {
    ChessBoard c;

    c.set_from_fen("6r1/5k1P/8/8/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::WHITE).len(), 8);
    c.set_from_fen("1r6/P4k2/8/8/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::WHITE).len(), 8);

    c.set_from_fen("8/5k2/8/8/8/8/p4K2/1R6 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::BLACK).len(), 8);
    c.set_from_fen("8/5k2/8/8/8/8/5K1p/6R1 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::BLACK).len(), 8);
}

TEST(MoveGeneratorTests, TestPawnCaptures) {
    ChessBoard c;

    c.set_from_fen("8/5k2/1r1r4/2P5/8/8/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::WHITE).len(), 3);

    c.set_from_fen("8/5k2/8/8/2p5/1R1R4/5K2/8 w - - 0 1");
    ASSERT_EQ(MoveGenerator::generate_pawn_moves(c, Side::BLACK).len(), 3);
}

TEST(MoveGeneratorTests, TestCastling) {
    ChessBoard c;
    c.set_kingside_castling(Side::WHITE, true);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::WHITE).len(), 1);
    // ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::WHITE)[0].get_move(), 8454);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKn1R w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::WHITE).len(), 0);

    c.set_queenside_castling(Side::WHITE, true);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::WHITE).len(), 1);
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1n1KBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::WHITE).len(), 0);

    c.set_kingside_castling(Side::BLACK, true);
    c.set_from_fen("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::BLACK).len(), 1);
    c.set_from_fen("rnbqkN1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::BLACK).len(), 0);

    c.set_queenside_castling(Side::WHITE, true);
    c.set_from_fen("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::BLACK).len(), 1);
    c.set_from_fen("r1N1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(MoveGenerator::generate_castling_moves(c, Side::BLACK).len(), 0);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipete) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    o.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    auto legal_moves = MoveGenerator::generate_legal_moves(c, Side::WHITE);

    auto rook_moves = MoveGenerator::generate_rook_moves(c, Side::WHITE);
    ASSERT_EQ(rook_moves.len(), 5);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, rook_moves).len(), 5);

    auto bishop_moves = MoveGenerator::generate_bishop_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, bishop_moves).len(), 11);

    auto king_moves = MoveGenerator::generate_king_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, king_moves).len(), 2);

    auto castling_moves = MoveGenerator::generate_castling_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, castling_moves).len(), 2);

    auto queen_moves = MoveGenerator::generate_queen_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, queen_moves).len(), 9);

    auto knight_moves = MoveGenerator::generate_knight_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, knight_moves).len(), 11);

    auto pawn_moves = MoveGenerator::generate_pawn_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, pawn_moves).len(), 8);

    ASSERT_EQ(legal_moves.len(), 48);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipeteB2B3) {
    ChessBoard c, o;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1");
    auto legal_moves = MoveGenerator::generate_legal_moves(c, Side::BLACK);

    auto rook_moves = MoveGenerator::generate_rook_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, rook_moves).len(), 9);

    auto bishop_moves = MoveGenerator::generate_bishop_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, bishop_moves).len(), 8);

    auto king_moves = MoveGenerator::generate_king_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, king_moves).len(), 2);

    auto castling_moves = MoveGenerator::generate_castling_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, castling_moves).len(), 2);

    auto queen_moves = MoveGenerator::generate_queen_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, queen_moves).len(), 4);

    auto knight_moves = MoveGenerator::generate_knight_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, knight_moves).len(), 10);

    auto pawn_moves = MoveGenerator::generate_pawn_moves(c, Side::BLACK);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, pawn_moves).len(), 7);

    ASSERT_EQ(legal_moves.len(), 42);
}

TEST(MoveGeneratorTests, TestCorrectMoveCountKiwipeteE1D1C7C6) {
    ChessBoard c, o;
    MoveHistory m;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1");
    c.make_move(Move(2738), m);

    auto legal_moves = MoveGenerator::generate_legal_moves(c, Side::WHITE);

    auto rook_moves = MoveGenerator::generate_rook_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, rook_moves).len(), 5);

    auto bishop_moves = MoveGenerator::generate_bishop_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, bishop_moves).len(), 11);

    auto king_moves = MoveGenerator::generate_king_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, king_moves).len(), 2);

    auto castling_moves = MoveGenerator::generate_castling_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, castling_moves).len(), 0);

    auto queen_moves = MoveGenerator::generate_queen_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, queen_moves).len(), 9);

    auto knight_moves = MoveGenerator::generate_knight_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, knight_moves).len(), 10);

    auto pawn_moves = MoveGenerator::generate_pawn_moves(c, Side::WHITE);
    ASSERT_EQ(MoveGenerator::filter_to_legal_moves(c, pawn_moves).len(), 9);

    ASSERT_EQ(legal_moves.len(), 46);
}

TEST(MoveGeneratorTests, TestRookMoves) {
    ChessBoard c;
    c.set_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto rook_moves = MoveGenerator::generate_rook_moves(c, Side::WHITE);
    auto legal_moves = MoveGenerator::filter_to_legal_moves(c, rook_moves);
    ASSERT_EQ(legal_moves.len(), 5);
}
