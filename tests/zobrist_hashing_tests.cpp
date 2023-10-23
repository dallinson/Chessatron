#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/zobrist_hashing.hpp"

TEST(ZobristHashingTests, TestSimplePosition) {
    ChessBoard c;
    c.set_from_fen("8/4k3/8/6K1/8/8/1P6/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::SideToMove);
    MoveHistory m;
    c.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 17)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::EnPassantKeys[1]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestEnPassant) {
    ChessBoard c;
    c.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::SideToMove);
    MoveHistory m;
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::EnPassantKeys[1]);
    c.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 17)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::EnPassantKeys[1]);
}

TEST(ZobristHashingTests, TestPawnPromotionAndCapture) {
    ChessBoard c;
    c.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
    MoveHistory m;
    c.make_move(Move(MoveFlags::ROOK_PROMOTION, 56, 48), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 56)]);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 53, 52), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 53)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 56)] ^
                                       ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::CAPTURE, 57, 56), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 53)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 57)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 53)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 56)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 56)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestPawnPromotionCapture) {
    ChessBoard c;
    c.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
    MoveHistory m;
    c.make_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 57)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestCastling) {
    ChessBoard c;
    c.set_from_fen("8/8/5k2/8/8/8/8/R3K2R w KQ - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    MoveHistory m;
    c.make_move(Move(MoveFlags::QUIET_MOVE, 12, 4), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 12)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 8, 0), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 8)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 15, 7), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 15)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[2]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2, 4), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 3)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 2)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::KINGSIDE_CASTLE, 6, 4), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 6)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 5)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestPolyglotExamples) {
    ChessBoard c;
    c.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x463b96181691fc9c);

    c.set_from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x823c9b50fd114196);

    c.set_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x0756b94461c50fb0);

    c.set_from_fen("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x662fafb965db29d4);

    c.set_from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x22a48b5a8e47ff78);

    c.set_from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x652a607ca3f242c1);

    c.set_from_fen("rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4");
    ASSERT_EQ(c.get_polyglot_zobrist_key(), 0x00fdd303c946bdd9);
}

TEST(ZobristHashingTests, TestEmptyBoard) {
    ChessBoard c;
    c.clear_board();
    ASSERT_EQ(c.get_occupancy(), 0);
    ASSERT_EQ(c.get_side_to_move(), Side::WHITE);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::SideToMove);
    ASSERT_EQ(c.get_polyglot_zobrist_key(), ZobristKeys::SideToMove);
}