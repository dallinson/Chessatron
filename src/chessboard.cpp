#include "chessboard.hpp"

#include <string>

#include <cstring>

void ChessBoard::set_piece(uint_fast8_t piece, uint_fast8_t pos) {
    pieces[pos] = piece;
    SET_BIT(bitboards[(((piece & PIECE_MASK) - 1) * 2) + GET_BIT(piece, 3)], pos);
}

void ChessBoard::clear_board() {
    for (int i = 0; i < 12; i++) {
        bitboards[i] = 0;
    }

    for (size_t i = 0; i < pieces.size(); i++) {
        pieces[i] = 0;
    }
}

void ChessBoard::print_board() const {
    static const char *piece_str = ".PRNBQK..prnbqk.";
    for (int_fast8_t rank = 7; rank >= 0; rank--) {
        for (uint_fast8_t file = 0; file < 8; file++) {
            printf("%c", piece_str[pieces[(rank * 8) + file].get_value()]);
        }
        printf("\n");
    }
}

void ChessBoard::set_from_fen(const char *input) {
    if (std::string(input, strlen(input)) == "startpos") {
        set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        return;
    }
    clear_board();
    int rank = 7;
    int file = 0;
    int char_idx = 0;
    while (input[char_idx] != ' ') {
        if (input[char_idx] == '/') {
            file = 0;
            rank -= 1;
        } else {
            char current = input[char_idx];
            uint_fast8_t piece = 0;
            if (current > 96) {
                piece += 8;
                current -= 32;
            }
            switch (current) {
            case 'P':
                piece += PAWN_VALUE;
                break;
            case 'R':
                piece += ROOK_VALUE;
                break;
            case 'N':
                piece += KNIGHT_VALUE;
                break;
            case 'B':
                piece += BISHOP_VALUE;
                break;
            case 'Q':
                piece += QUEEN_VALUE;
                break;
            case 'K':
                piece += KING_VALUE;
                break;
            default:
                file += (current - 48);
                char_idx += 1;
                continue;
            }
            set_piece(piece, (rank * 8) + file);
            file += 1;
        }
        char_idx += 1;
    }
}

int ChessBoard::get_score(int side) {
    return (_mm_popcnt_u64(get_pawn_occupancy(side)) * PAWN_VALUE) + (_mm_popcnt_u64(get_rook_occupancy(side)) * ROOK_VALUE) + (_mm_popcnt_u64(get_knight_occupancy(side)) * KNIGHT_VALUE) + (_mm_popcnt_u64(get_bishop_occupancy(side)) * BISHOP_VALUE) + (_mm_popcnt_u64(get_queen_occupancy(side)) * QUEEN_VALUE);
}

void ChessBoard::make_move(const Move to_make, MoveHistory& move_history) {
    Piece moved = this->pieces[to_make.get_src_square()];
    const int side = moved.get_side();
    this->pieces[to_make.get_src_square()] = 0;
    // get the piece we're moving and clear the origin square
    Piece at_target = this->pieces[to_make.get_dest_square()];
    this->pieces[to_make.get_dest_square()] = moved;
    // get the piece we replace with ourselves and do the replacement
    auto to_add = std::make_pair(to_make, PreviousMoveState(at_target, this->get_en_passant_file(), get_kingside_castling(WHITE_IDX), get_queenside_castling(WHITE_IDX), get_kingside_castling(BLACK_IDX), get_queenside_castling(BLACK_IDX)));
    move_history.push_move(to_add);

    CLEAR_BIT(this->bitboards[moved.to_bitboard_idx()], to_make.get_src_square());
    if (at_target.get_value()) {
        // If there _was_ a piece there
        // we do this as en passant captures without a piece at the position
        CLEAR_BIT(this->bitboards[at_target.to_bitboard_idx()], to_make.get_dest_square());
    }

    if (to_make.get_move_flags() & 0x8) {
        // Any value >= 8 is a promotion
        int promoted_piece = ((to_make.get_move_flags() & 0x3) + 2) * 2;
        promoted_piece += side;
        SET_BIT(this->bitboards[promoted_piece], to_make.get_dest_square());
        // This handles pawn promotions
    } else {
        SET_BIT(this->bitboards[moved.to_bitboard_idx()], to_make.get_dest_square());
        // otherwise sets pieces if moved normally
    }

    if (to_make.get_move_flags() == DOUBLE_PAWN_PUSH) {
        this->en_passant_file = to_make.get_dest_file();
    } else {
        this->en_passant_file = 9;
    }
    // set where the last en passant happened, else clear it

    if (to_make.get_move_flags() == EN_PASSANT_CAPTURE) {
        int enemy_side = (side + 1) & 0x1;
        int enemy_pawn_idx = to_make.get_dest_square() - 8 + (16 * side);
        CLEAR_BIT(this->bitboards[PAWN_OFFSET + enemy_side], enemy_pawn_idx);
    }

    if (to_make.get_move_flags() == KINGSIDE_CASTLE) {
        // we moved the king, now move the rook
        this->pieces[to_make.get_dest_square() - 1] = this->pieces[to_make.get_dest_square() + 1];
        this->pieces[to_make.get_dest_square() + 1] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + side], to_make.get_dest_square() + 1);
        SET_BIT(this->bitboards[ROOK_OFFSET + side], to_make.get_dest_square() - 1);
    }
    if (to_make.get_move_flags() == QUEENSIDE_CASTLE) {
        this->pieces[to_make.get_dest_square() + 1] = this->pieces[to_make.get_dest_square() - 2];
        this->pieces[to_make.get_dest_square() - 2] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + side], to_make.get_dest_square() - 2);
        SET_BIT(this->bitboards[ROOK_OFFSET + side], to_make.get_dest_square() + 1);
    }

    if (moved.get_piece_value() == KING_VALUE) {
        set_kingside_castling(side, false);
        set_queenside_castling(side, false);
    }
    if (to_make.get_src_square() == 0 || to_make.get_dest_square() == 0) {
        set_queenside_castling(0, false);
    }
    if (to_make.get_src_square() == 7 || to_make.get_dest_square() == 7) {
        set_kingside_castling(0, false);
    }

    if (to_make.get_src_square() == 56 || to_make.get_dest_square() == 56) {
        set_queenside_castling(0, false);
    }
    if (to_make.get_src_square() == 63 || to_make.get_dest_square() == 63) {
        set_kingside_castling(0, false);
    }
}

void ChessBoard::unmake_move(MoveHistory& move_history) {
    const std::pair<Move, PreviousMoveState> previous_move_pair = move_history.pop_move();
    Piece original = pieces[previous_move_pair.first.get_dest_square()];
    const int side = original.get_side();
    CLEAR_BIT(this->bitboards[original.to_bitboard_idx()], previous_move_pair.first.get_dest_square());
    pieces[previous_move_pair.first.get_dest_square()] = previous_move_pair.second.get_piece();
    if (previous_move_pair.first.get_move_flags() >= 8) {
        // if it's a promotion
        Piece new_piece = Piece(original.get_side(), PAWN_VALUE);
        pieces[previous_move_pair.first.get_src_square()] = new_piece;
        SET_BIT(this->bitboards[new_piece.to_bitboard_idx()], previous_move_pair.first.get_src_square());
    } else {
        pieces[previous_move_pair.first.get_src_square()] = original;
        SET_BIT(this->bitboards[original.to_bitboard_idx()], previous_move_pair.first.get_src_square());
    }

    if (previous_move_pair.first.get_move_flags() & 4) {
        if (previous_move_pair.first.get_move_flags() != EN_PASSANT_CAPTURE) {
            // if it is a capture
            SET_BIT(this->bitboards[previous_move_pair.second.get_piece().to_bitboard_idx()], previous_move_pair.first.get_dest_square());
        } else {
            const int enemy_side = (side + 1) & 1;
            const int enemy_pawn_idx = previous_move_pair.first.get_dest_square() - 8 + (16 * side);
            this->pieces[enemy_pawn_idx] = Piece(enemy_side, PAWN_VALUE);
            SET_BIT(this->bitboards[PAWN_OFFSET + side], enemy_pawn_idx);
        }
    }

    if (previous_move_pair.first.get_move_flags() == KINGSIDE_CASTLE) {
        // We need to unmove the rook
        const int origin_square = previous_move_pair.first.get_dest_square();
        this->pieces[origin_square - 1] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + side], origin_square - 1);
        this->pieces[origin_square + 1] = Piece(side, ROOK_VALUE);
        SET_BIT(this->bitboards[ROOK_OFFSET + side], origin_square + 1);
    } else if (previous_move_pair.first.get_move_flags() == QUEENSIDE_CASTLE) {
        const int origin_square = previous_move_pair.first.get_dest_square();
        this->pieces[origin_square + 1] = 0;
        CLEAR_BIT(this->bitboards[ROOK_OFFSET + side], origin_square + 1);
        this->pieces[origin_square - 2] = Piece(side, ROOK_VALUE);
        SET_BIT(this->bitboards[ROOK_OFFSET - side], origin_square - 2);
    }

    set_en_passant_file(previous_move_pair.second.get_previous_en_passant_file());
    set_kingside_castling(0, previous_move_pair.second.get_white_kingside_castle());
    set_queenside_castling(0, previous_move_pair.second.get_white_queenside_castle());
    set_kingside_castling(1, previous_move_pair.second.get_black_kingside_castle());
    set_queenside_castling(1, previous_move_pair.second.get_black_queenside_castle());
}