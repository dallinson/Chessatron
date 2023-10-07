#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/zobrist_hashing.hpp"

TEST(ZobristHashingTests, TestSimplePosition) {
    ChessBoard c;
    c.set_from_fen("8/4k3/8/6K1/8/8/1P6/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)]);
    MoveHistory m;
    c.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 17)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)]);
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::SideToMove ^ ZobristKeys::EnPassantKeys[1]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)]);
}

TEST(ZobristHashingTests, TestEnPassant) {
    ChessBoard c;
    c.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 26)]);
    MoveHistory m;
    c.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 26)] ^
                                       ZobristKeys::SideToMove ^ ZobristKeys::EnPassantKeys[1]);
    c.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 17)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 26)] ^
                                       ZobristKeys::SideToMove ^ ZobristKeys::EnPassantKeys[1]);
}

TEST(ZobristHashingTests, TestPawnPromotionAndCapture) {
    ChessBoard c;
    c.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 48)]);
    MoveHistory m;
    c.make_move(Move(MoveFlags::ROOK_PROMOTION, 56, 48), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 56)] ^
                                       ZobristKeys::SideToMove);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 53, 52), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 53)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 56)]);
    c.make_move(Move(MoveFlags::CAPTURE, 57, 56), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 53)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 57)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 53)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 56)]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 56)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 48)]);
}

TEST(ZobristHashingTests, TestPawnPromotionCapture) {
    ChessBoard c;
    c.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 48)]);
    MoveHistory m;
    c.make_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 57)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 38)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 52)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::PAWN), 48)]);
}

TEST(ZobristHashingTests, TestCastling) {
    ChessBoard c;
    c.set_from_fen("8/8/5k2/8/8/8/8/R3K2R w KQ - 0 1");
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2]);
    MoveHistory m;
    c.make_move(Move(MoveFlags::QUIET_MOVE, 12, 4), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 12)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2]);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 8, 0), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 8)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::SideToMove ^ ZobristKeys::CastlingKeys[0]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2]);
    c.make_move(Move(MoveFlags::QUIET_MOVE, 15, 7), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 15)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::SideToMove ^ ZobristKeys::CastlingKeys[2]);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2]);
    c.make_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2, 4), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 3)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 2)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2]);
    c.make_move(Move(MoveFlags::KINGSIDE_CASTLE, 6, 4), m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 6)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 5)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::SideToMove);
    c.unmake_move(m);
    ASSERT_EQ(c.get_zobrist_key(), ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::KING), 4)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::WHITE, PieceValues::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(Side::BLACK, PieceValues::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2]);
}