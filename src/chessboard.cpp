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

void ChessBoard::make_move(const Move to_make, const int side) {
    Piece moved = this->pieces[to_make.get_src_square()];
    this->pieces[to_make.get_src_square()] = 0;
    // get the piece we're moving and clear the origin square
    Piece at_target = this->pieces[to_make.get_dest_square()];
    this->pieces[to_make.get_dest_square()] = moved;
    // get the piece we replace with ourselves and do the replacement

    CLEAR_BIT(this->bitboards[moved.to_bitboard_idx()], to_make.get_src_square());
    if (at_target.get_value()) {
        // If there _was_ a piece there
        // we do this as en passant captures without a piece at the position
        CLEAR_BIT(this->bitboards[at_target.to_bitboard_idx()], to_make.get_dest_square());
    }

    if (to_make.get_move_flags() & 0x8) {
        // Any value >= 8 is a promotion
        int promoted_piece = ((to_make.get_move_flags() & 0x3) + 2) * 2;
        promoted_piece += moved.get_side();
        SET_BIT(this->bitboards[promoted_piece], to_make.get_dest_square());
        // This handles pawn promotions
    } else {
        SET_BIT(this->bitboards[moved.to_bitboard_idx()], to_make.get_dest_square());
        // otherwise sets pieces if moved normally
    }

    if (to_make.get_move_flags() == DOUBLE_PAWN_PUSH) {
        this->en_passant_file = to_make.get_dest_file();
    } else {
        this->en_passant_file = 96;
    }

    if (to_make.get_move_flags() == EN_PASSANT_CAPTURE) {
        int enemy_side = (side + 1) & 0x1;
        int enemy_pawn_idx = to_make.get_dest_square() - 8 + (16 * side);
        CLEAR_BIT(this->bitboards[PAWN_OFFSET + side], enemy_pawn_idx);
    }


}