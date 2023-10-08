#include "chessboard.hpp"

#include <bit>
#include <cstring>
#include <string>

#include "magic_numbers.hpp"
#include "move_generator.hpp"
#include "zobrist_hashing.hpp"

void ChessBoard::set_piece(Piece piece, uint_fast8_t pos) {
    pieces[pos] = piece;
    SET_BIT(bitboards[piece.to_bitboard_idx()], pos);
}

void ChessBoard::clear_board() {
    for (int i = 0; i < 12; i++) {
        bitboards[i] = 0;
    }

    for (size_t i = 0; i < pieces.size(); i++) {
        pieces[i] = 0;
    }

    side_to_move = Side::WHITE;
    en_passant_file = 9;
    for (int i = 0; i < 4; i++) {
        castling[i] = false;
    }
}

void ChessBoard::print_board() const {
    static const char* piece_str = ".PRNBQK..prnbqk.";
    for (int_fast8_t rank = 7; rank >= 0; rank--) {
        for (uint_fast8_t file = 0; file < 8; file++) {
            printf("%c", piece_str[pieces[(rank * 8) + file].get_value()]);
        }
        printf("\n");
    }
}

#define RETURN_FALSE_IF_PAST_END                                                                                                                     \
    if ((size_t) char_idx >= strlen(input)) {                                                                                                        \
        return false;                                                                                                                                \
    }

bool ChessBoard::set_from_fen(const char* input) {
    if (std::string(input, strlen(input)) == "startpos") {
        return set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    clear_board();
    int rank = 7;
    int file = 0;
    int char_idx = 0;
    // set up the board
    while (input[char_idx] != ' ') {
        if (input[char_idx] == '/') {
            file = 0;
            rank -= 1;
            if (rank < 0) {
                return false;
            }
        } else {
            char current = input[char_idx];
            Side piece_side = Side::WHITE;
            PieceValues piece_value = PieceValues::PAWN;
            if (current > 96) {
                piece_side = Side::BLACK;
                current -= 32;
            }
            switch (current) {
            case 'P':
                piece_value = PieceValues::PAWN;
                break;
            case 'R':
                piece_value = PieceValues::ROOK;
                break;
            case 'N':
                piece_value = PieceValues::KNIGHT;
                break;
            case 'B':
                piece_value = PieceValues::BISHOP;
                break;
            case 'Q':
                piece_value = PieceValues::QUEEN;
                break;
            case 'K':
                piece_value = PieceValues::KING;
                break;
            case '1':
                [[fallthrough]];
            case '2':
                [[fallthrough]];
            case '3':
                [[fallthrough]];
            case '4':
                [[fallthrough]];
            case '5':
                [[fallthrough]];
            case '6':
                [[fallthrough]];
            case '7':
                [[fallthrough]];
            case '8':
                file += (current - 48);
                char_idx += 1;
                continue;

            default:
                return false;
            }
            Piece piece = Piece(piece_side, piece_value);
            set_piece(piece, POSITION(rank, file));
            // zobrist_key ^= ZobristKeys::PositionKeys[(piece * 64) + POSITION(rank, file)];
            zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(piece, POSITION(rank, file))];
            file += 1;
            if (file > 8) {
                return false;
            }
        }
        char_idx += 1;
        if (get_king_occupancy(Side::WHITE) && get_king_occupancy(Side::BLACK)) {
            // prevents array index out of range exception
            recompute_blockers_and_checkers();
        }
        RETURN_FALSE_IF_PAST_END;
    }
    char_idx += 1;
    recompute_blockers_and_checkers();
    RETURN_FALSE_IF_PAST_END;
    // set whose turn it is
    if (input[char_idx] == 'w') {
        side_to_move = Side::WHITE;
    } else if (input[char_idx] == 'b') {
        side_to_move = Side::BLACK;
        zobrist_key ^= ZobristKeys::SideToMove;
    } else {
        return false;
    }
    char_idx += 2;
    // set castling
    while (input[char_idx] != ' ') {
        RETURN_FALSE_IF_PAST_END;
        switch (input[char_idx]) {
        case 'K':
            set_kingside_castling(Side::WHITE, true);
            break;
        case 'Q':
            set_queenside_castling(Side::WHITE, true);
            break;
        case 'k':
            set_kingside_castling(Side::BLACK, true);
            break;
        case 'q':
            set_queenside_castling(Side::BLACK, true);
            break;
        case '-':
            break;
        default:
            return false;
        }
        char_idx += 1;
    }
    char_idx += 1;

    RETURN_FALSE_IF_PAST_END;
    if (input[char_idx] == '-') {
        set_en_passant_file(9);
        char_idx += 1;
    } else {
        if (input[char_idx] >= 'a' && input[char_idx] <= 'h') {
            set_en_passant_file(input[char_idx] - 97);
            // in ascii 'a' has the value 97, so we subtract 97 to get the index of the file
            char_idx += 1;
        } else {
            return false;
        }
    }

    return true;
}

int ChessBoard::get_score(Side side) {
    return (std::popcount(get_pawn_occupancy(side)) * PAWN_SCORE) + (std::popcount(get_rook_occupancy(side)) * ROOK_SCORE) +
           (std::popcount(get_knight_occupancy(side)) * KNIGHT_SCORE) + (std::popcount(get_bishop_occupancy(side)) * BISHOP_SCORE) +
           (std::popcount(get_queen_occupancy(side)) * QUEEN_SCORE);
}

void ChessBoard::make_move(const Move to_make, MoveHistory& move_history) {
    Piece moved = this->pieces[to_make.get_src_square()];
    const Side side = moved.get_side();
    this->pieces[to_make.get_src_square()] = 0;
    zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(moved, to_make.get_src_square())];
    // get the piece we're moving and clear the origin square
    Piece at_target = this->pieces[to_make.get_dest_square()];

    // get the piece we replace with ourselves and do the replacement
    auto to_add = std::make_pair(to_make, PreviousMoveState(at_target, this->get_en_passant_file(), get_kingside_castling(Side::WHITE),
                                                            get_queenside_castling(Side::WHITE), get_kingside_castling(Side::BLACK),
                                                            get_queenside_castling(Side::BLACK)));
    move_history.push_move(to_add);

    CLEAR_BIT(this->bitboards[moved.to_bitboard_idx()], to_make.get_src_square());
    if (at_target.get_value()) {
        // If there _was_ a piece there
        // we do this as en passant captures without a piece at the position
        CLEAR_BIT(this->bitboards[at_target.to_bitboard_idx()], to_make.get_dest_square());
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(at_target, to_make.get_dest_square())];
    }

    if (static_cast<int>(to_make.get_move_flags()) >= 8) {
        // Any value >= 8 is a promotion
        Piece promoted_piece = Piece(side, PieceValues((static_cast<int>(to_make.get_move_flags()) & 0b0011) + 2));
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(promoted_piece, to_make.get_dest_square())];
        this->pieces[to_make.get_dest_square()] = promoted_piece;
        // promoted_piece += side;
        SET_BIT(this->bitboards[promoted_piece.to_bitboard_idx()], to_make.get_dest_square());
        // This handles pawn promotions
    } else {
        SET_BIT(this->bitboards[moved.to_bitboard_idx()], to_make.get_dest_square());
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(moved, to_make.get_dest_square())];
        this->pieces[to_make.get_dest_square()] = moved;
        // otherwise sets pieces if moved normally
    }

    this->zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
    if (to_make.get_move_flags() == MoveFlags::DOUBLE_PAWN_PUSH) [[unlikely]] {
        this->en_passant_file = to_make.get_dest_file();
        this->zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
    } else {
        this->en_passant_file = 9;
        // the en passant zobrist key for 9 is 0 so no need to XOR (would be a no-op)
    }
    // set where the last en passant happened, else clear it

    if (to_make.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) [[unlikely]] {
        Side enemy_side = ENEMY_SIDE(side);
        int enemy_pawn_idx = to_make.get_dest_square() - 8 + (16 * static_cast<int>(side));
        CLEAR_BIT(this->bitboards[PAWN_OFFSET + static_cast<int>(enemy_side)], enemy_pawn_idx);
        this->pieces[enemy_pawn_idx] = 0;
        this->zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(enemy_side, PieceValues::PAWN), enemy_pawn_idx)];
    }

    if (to_make.get_move_flags() == MoveFlags::KINGSIDE_CASTLE) {
        // we moved the king, now move the rook
        this->pieces[to_make.get_dest_square() - 1] = this->pieces[to_make.get_dest_square() + 1];
        this->pieces[to_make.get_dest_square() + 1] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(side)], to_make.get_dest_square() + 1);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(side, PieceValues::ROOK), to_make.get_dest_square() + 1)];
        SET_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(side)], to_make.get_dest_square() - 1);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(side, PieceValues::ROOK), to_make.get_dest_square() - 1)];
    }
    if (to_make.get_move_flags() == MoveFlags::QUEENSIDE_CASTLE) {
        this->pieces[to_make.get_dest_square() + 1] = this->pieces[to_make.get_dest_square() - 2];
        this->pieces[to_make.get_dest_square() - 2] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(side)], to_make.get_dest_square() - 2);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(side, PieceValues::ROOK), to_make.get_dest_square() - 2)];
        SET_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(side)], to_make.get_dest_square() + 1);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(side, PieceValues::ROOK), to_make.get_dest_square() + 1)];
    }

    if (moved.get_type() == PieceValues::KING) {
        set_kingside_castling(side, false);
        set_queenside_castling(side, false);
    }
    if (to_make.get_src_square() == 0 || to_make.get_dest_square() == 0) {
        set_queenside_castling(Side::WHITE, false);
    }
    if (to_make.get_src_square() == 7 || to_make.get_dest_square() == 7) {
        set_kingside_castling(Side::WHITE, false);
    }

    if (to_make.get_src_square() == 56 || to_make.get_dest_square() == 56) {
        set_queenside_castling(Side::BLACK, false);
    }
    if (to_make.get_src_square() == 63 || to_make.get_dest_square() == 63) {
        set_kingside_castling(Side::BLACK, false);
    }

    side_to_move = ENEMY_SIDE(side_to_move);
    zobrist_key ^= ZobristKeys::SideToMove;
    recompute_blockers_and_checkers(side_to_move);
}

void ChessBoard::unmake_move(MoveHistory& move_history) {
    const std::pair<Move, PreviousMoveState> previous_move_pair = move_history.pop_move();
    Piece original = pieces[previous_move_pair.first.get_dest_square()];
    const Side moved_piece_side = original.get_side();
    CLEAR_BIT(this->bitboards[original.to_bitboard_idx()], previous_move_pair.first.get_dest_square());
    zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(original, previous_move_pair.first.get_dest_square())];
    // this unsets the target piece
    pieces[previous_move_pair.first.get_dest_square()] = previous_move_pair.second.get_piece();

    if (static_cast<int>(previous_move_pair.first.get_move_flags()) >= 8) {
        // if it's a promotion
        Piece new_piece = Piece(original.get_side(), PieceValues::PAWN);
        pieces[previous_move_pair.first.get_src_square()] = new_piece;
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(new_piece, previous_move_pair.first.get_src_square())];
        SET_BIT(this->bitboards[new_piece.to_bitboard_idx()], previous_move_pair.first.get_src_square());
    } else {
        pieces[previous_move_pair.first.get_src_square()] = original;
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(original, previous_move_pair.first.get_src_square())];
        SET_BIT(this->bitboards[original.to_bitboard_idx()], previous_move_pair.first.get_src_square());
    }

    if (static_cast<int>(previous_move_pair.first.get_move_flags()) & 0b0100) {
        if (previous_move_pair.first.get_move_flags() != MoveFlags::EN_PASSANT_CAPTURE) {
            // if it is a capture
            SET_BIT(this->bitboards[previous_move_pair.second.get_piece().to_bitboard_idx()], previous_move_pair.first.get_dest_square());
            zobrist_key ^=
                ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(previous_move_pair.second.get_piece(), previous_move_pair.first.get_dest_square())];
        } else {
            const Side enemy_side = ENEMY_SIDE(moved_piece_side);
            const int enemy_pawn_idx = previous_move_pair.first.get_dest_square() - 8 + (16 * static_cast<int>(moved_piece_side));
            this->pieces[enemy_pawn_idx] = Piece(enemy_side, PieceValues::PAWN);
            zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(this->pieces[enemy_pawn_idx], enemy_pawn_idx)];
            SET_BIT(this->bitboards[PAWN_OFFSET + static_cast<int>(enemy_side)], enemy_pawn_idx);
        }
    }

    if (previous_move_pair.first.get_move_flags() == MoveFlags::KINGSIDE_CASTLE) {
        // We need to unmove the rook
        const int origin_square = previous_move_pair.first.get_dest_square();
        this->pieces[origin_square - 1] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(moved_piece_side)], origin_square - 1);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(moved_piece_side, PieceValues::ROOK), origin_square - 1)];
        this->pieces[origin_square + 1] = Piece(moved_piece_side, PieceValues::ROOK);
        SET_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(moved_piece_side)], origin_square + 1);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(moved_piece_side, PieceValues::ROOK), origin_square + 1)];
    } else if (previous_move_pair.first.get_move_flags() == MoveFlags::QUEENSIDE_CASTLE) {
        const int origin_square = previous_move_pair.first.get_dest_square();
        this->pieces[origin_square + 1] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(moved_piece_side)], origin_square + 1);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(moved_piece_side, PieceValues::ROOK), origin_square + 1)];
        this->pieces[origin_square - 2] = Piece(moved_piece_side, PieceValues::ROOK);
        SET_BIT(this->bitboards[ROOK_OFFSET + static_cast<int>(moved_piece_side)], origin_square - 2);
        zobrist_key ^= ZobristKeys::PositionKeys[ZOBRIST_POSITION_KEY(Piece(moved_piece_side, PieceValues::ROOK), origin_square - 2)];
    }

    set_en_passant_file(previous_move_pair.second.get_previous_en_passant_file());
    set_kingside_castling(Side::WHITE, previous_move_pair.second.get_white_kingside_castle());
    set_queenside_castling(Side::WHITE, previous_move_pair.second.get_white_queenside_castle());
    set_kingside_castling(Side::BLACK, previous_move_pair.second.get_black_kingside_castle());
    set_queenside_castling(Side::BLACK, previous_move_pair.second.get_black_queenside_castle());

    side_to_move = ENEMY_SIDE(side_to_move);
    zobrist_key ^= ZobristKeys::SideToMove;
    recompute_blockers_and_checkers(side_to_move);
}

void ChessBoard::recompute_blockers_and_checkers(const Side side) {
    const int king_idx = bitboard_to_idx(this->get_king_occupancy(side));
    const Side enemy_side = ENEMY_SIDE(side);
    checkers[static_cast<int>(side)] = MoveGenerator::get_checkers(*this, side, king_idx);

    pinned_pieces[static_cast<int>(side)] = 0;

    Bitboard potential_checks =
        ((MoveGenerator::generate_bishop_movemask(0, king_idx) & (get_bishop_occupancy(enemy_side) | get_queen_occupancy(enemy_side))) |
         (MoveGenerator::generate_rook_movemask(0, king_idx) & (get_rook_occupancy(enemy_side) | get_queen_occupancy(enemy_side)))) ^
        checkers[static_cast<int>(side)];
    const Bitboard check_blockers = get_occupancy() ^ potential_checks;
    // don't evaluate ones where we already check the king

    while (potential_checks) {
        const Bitboard line_to_king = MagicNumbers::ConnectingSquares[king_idx][pop_min_bit(potential_checks)];
        if (std::popcount(line_to_king & check_blockers) <= 1) {
            pinned_pieces[static_cast<int>(side)] |= line_to_king & check_blockers;
        }
    }
}

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs) {
    bool is_equal = true;
    for (int i = 0; i < 64; i++) {
        is_equal &= (lhs.get_piece(i) == rhs.get_piece(i));
    }

    for (int i = 0; i < 12; i++) {
        is_equal &= (lhs.get_bitboard(i) == rhs.get_bitboard(i));
        // this just makes it a bit nicer to use
    }

    is_equal &= (lhs.get_en_passant_file() == rhs.get_en_passant_file());

    for (int i = 0; i < 2; i++) {
        Side s = Side(i);
        is_equal &= (lhs.get_queenside_castling(s) == rhs.get_queenside_castling(s));
        is_equal &= (lhs.get_kingside_castling(s) == rhs.get_kingside_castling(s));
    }

    return is_equal;
}