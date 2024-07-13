#include <gtest/gtest.h>

#include "../src/chessboard.hpp"
#include "../src/zobrist_hashing.hpp"

TEST(ZobristHashingTests, TestSimplePosition) {
    Position pos;
    pos.set_from_fen("8/4k3/8/6K1/8/8/1P6/8 w - - 0 1");
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 17)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::SideToMove);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::EnPassantKeys[1]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestSimplePositionPawnHash) {
    Position pos;
    pos.set_from_fen("8/4k3/8/6K1/8/8/1P6/8 w - - 0 1");
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^ ZobristKeys::SideToMove);

    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 17, 9), hist);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 17)]);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^ ZobristKeys::SideToMove);

    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), hist);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 25)]);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^ ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestEnPassant) {
    Position pos;
    pos.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 0 1");
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::EnPassantKeys[1]);
    pos = pos.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 17)] ^
                                       ZobristKeys::SideToMove);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::EnPassantKeys[1]);
}

TEST(ZobristHashingTests, TestEnPassantPawnHash) {
    Position pos;
    pos.set_from_fen("8/4k3/8/6K1/2p5/8/1P6/8 w - - 0 1");
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 9)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 26)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::DOUBLE_PAWN_PUSH, 25, 9), hist);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 26)]);

    pos = pos.make_move(Move(MoveFlags::EN_PASSANT_CAPTURE, 17, 26), hist);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 17)] ^ ZobristKeys::SideToMove);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 25)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 26)]);

}

TEST(ZobristHashingTests, TestPawnPromotionAndCapture) {
    Position pos;
    pos.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::ROOK_PROMOTION, 56, 48), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 56)]);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 53, 52), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 53)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 56)] ^
                                       ZobristKeys::SideToMove);
    pos = pos.make_move(Move(MoveFlags::CAPTURE, 57, 56), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 53)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 57)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 53)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 56)] ^
                                       ZobristKeys::SideToMove);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 56)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestPawnPromotionAndCapturePawnHash) {
    Position pos;
    pos.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::ROOK_PROMOTION, 56, 48), hist);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)]);

    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 53, 52), hist);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^ ZobristKeys::SideToMove);

    pos = pos.make_move(Move(MoveFlags::CAPTURE, 57, 56), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^ ZobristKeys::SideToMove);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)]);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
}


TEST(ZobristHashingTests, TestPawnPromotionCapture) {
    Position pos;
    pos.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 57)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 38)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 52)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestPawnPromotionCapturePawnHash) {
    Position pos;
    pos.set_from_fen("1p6/P3k3/8/6K1/8/8/8/8 w - - 0 1");
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::ROOK_PROMOTION_CAPTURE, 57, 48), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::PAWN), 57)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::PAWN), 48)] ^
                                       ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestCastling) {
    Position pos;
    pos.set_from_fen("8/8/5k2/8/8/8/8/R3K2R w KQ - 0 1");
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 12, 4), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 12)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 8, 0), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 8)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 15, 7), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 15)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[2]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    pos = pos.make_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2, 4), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 3)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 2)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
    pos = pos.make_move(Move(MoveFlags::KINGSIDE_CASTLE, 6, 4), hist);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 6)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 5)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)]);
    pos = hist.pop_board();
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 0)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::KING), 4)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::WHITE, PieceTypes::ROOK), 7)] ^
                                       ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(Side::BLACK, PieceTypes::KING), 45)] ^
                                       ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestCastlingPawnHash) {
    Position pos;
    pos.set_from_fen("8/8/5k2/8/8/8/8/R3K2R w KQ - 0 1");
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);

    BoardHistory hist(pos);
    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 12, 4), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);

    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 8, 0), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);

    pos = pos.make_move(Move(MoveFlags::QUIET_MOVE, 15, 7), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);

    pos = pos.make_move(Move(MoveFlags::QUEENSIDE_CASTLE, 2, 4), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);

    pos = pos.make_move(Move(MoveFlags::KINGSIDE_CASTLE, 6, 4), hist);
    ASSERT_EQ(pos.pawn_hash(), 0);

    pos = hist.pop_board();
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);
}

TEST(ZobristHashingTests, TestPolyglotExamples) {
    Position pos;
    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x463b96181691fc9c);

    pos.set_from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x823c9b50fd114196);

    pos.set_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x0756b94461c50fb0);

    pos.set_from_fen("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x662fafb965db29d4);

    pos.set_from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x22a48b5a8e47ff78);

    pos.set_from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x652a607ca3f242c1);

    pos.set_from_fen("rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4");
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), 0x00fdd303c946bdd9);
}

TEST(ZobristHashingTests, TestEmptyBoard) {
    Position pos;
    pos.clear_board();
    ASSERT_TRUE(pos.occupancy().empty());
    ASSERT_EQ(pos.stm(), Side::WHITE);
    ASSERT_EQ(pos.zobrist_key(), ZobristKeys::SideToMove);
    ASSERT_EQ(pos.get_polyglot_zobrist_key(), ZobristKeys::SideToMove);
    ASSERT_EQ(pos.pawn_hash(), ZobristKeys::SideToMove);
}