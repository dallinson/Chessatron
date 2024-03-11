#include "chessboard.hpp"

#include <bit>
#include <cstring>
#include <string>

#include "magic_numbers.hpp"
#include "magic_numbers/piece_square_tables.hpp"
#include "move_generator.hpp"
#include "zobrist_hashing.hpp"

using namespace PieceSquareTables;
using enum PieceTypes;

constexpr static std::array<uint8_t, 6> mg_phase_vals = { 0, 1, 1, 2, 4, 0 };
const static std::array<ZobristKey, 16> castling_keys = {
    0, // 0000
    ZobristKeys::CastlingKeys[0], // 0001
    ZobristKeys::CastlingKeys[1], // 0010
    ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 0011

    ZobristKeys::CastlingKeys[2], // 0100
    ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0], // 0101
    ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[1], // 0110
    ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 0111

    ZobristKeys::CastlingKeys[3], // 1000
	ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[0], // 1001
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[1], // 1010
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 1011

    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2], // 1100
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0], // 1101
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[1], // 1110
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 1111
};
// Bit 0 is white kingside castling
// Bit 1 is black kingside castling
// Bit 2 is white queenside castling
// Bit 3 is black kingside castline

constexpr int castling_rights[64] = {
    0b1011, 15, 15, 15, 0b1010,  15, 15,  0b1110,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    0b0111, 15, 15, 15, 0b0101, 15, 15, 0b1101
};

void ChessBoard::set_piece(Piece piece, uint8_t pos) {
    pieces[pos] = piece;
    set_bit(bbs[piece.to_bitboard_idx()], pos);
    zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(piece, pos)];

    const auto piece_side = piece.get_side();
    const auto piece_type = piece.get_type();
    if (piece_side == Side::WHITE) {
        pos ^= 0b00111000;
    }

    scores[static_cast<int>(piece_side)] += Tables[static_cast<int>(piece_type) - 1][pos];

    mg_phase += mg_phase_vals[static_cast<int>(piece_type) - 1];
}

void ChessBoard::clear_board() {
    for (int i = 0; i < 12; i++) {
        bbs[i] = 0;
    }

    for (size_t i = 0; i < pieces.size(); i++) {
        pieces[i] = 0;
    }

    side_to_move = Side::WHITE;
    en_passant_file = 9;
    castling = 0;

    zobrist_key = ZobristKeys::SideToMove;

    scores = {0};
    // Only the white side to move key should be set
    mg_phase = 0;
}

void ChessBoard::print_board() const {
    static const char* piece_str = ".PNBRQK..pnbrqk.";
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            printf("%c", piece_str[pieces[(rank * 8) + file].get_value()]);
        }
        printf("\n");
    }
}

#define RETURN_NONE_IF_PAST_END                                                                                                                      \
    if ((size_t) char_idx >= (input).size()) {                                                                                                       \
        return std::optional<int>();                                                                                                                 \
    }

std::optional<int> ChessBoard::set_from_fen(const std::string input) {
    if (input.find("startpos") == 0) {
        // this accounts for any additional moves after the startpos
        set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        return std::optional<int>(8);
        // this ensures we won't read past the end when looking for moves
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
            PieceTypes piece_value = PAWN;
            if (current > 96) {
                piece_side = Side::BLACK;
                current -= 32;
            }
            switch (current) {
            case 'P':
                piece_value = PAWN;
                break;
            case 'R':
                piece_value = ROOK;
                break;
            case 'N':
                piece_value = KNIGHT;
                break;
            case 'B':
                piece_value = BISHOP;
                break;
            case 'Q':
                piece_value = QUEEN;
                break;
            case 'K':
                piece_value = KING;
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
                return std::optional<int>();
            }
            Piece piece = Piece(piece_side, piece_value);
            set_piece(piece, get_position(rank, file));
            // zobrist_key ^= ZobristKeys::PositionKeys[(piece * 64) + get_position(rank, file)];
            file += 1;
            if (file > 8) {
                return std::optional<int>();
            }
        }
        char_idx += 1;
        RETURN_NONE_IF_PAST_END;
    }
    char_idx += 1;
    RETURN_NONE_IF_PAST_END;
    // set whose turn it is
    if (input[char_idx] == 'w') {
        side_to_move = Side::WHITE;
    } else if (input[char_idx] == 'b') {
        side_to_move = Side::BLACK;
        zobrist_key ^= ZobristKeys::SideToMove;
        // Clearing the board sets this key, so this ensures the zobrist key is correct
    } else {
        return std::optional<int>();
    }
    recompute_blockers_and_checkers(side_to_move);
    RETURN_NONE_IF_PAST_END;
    char_idx += 2;
    // set castling
    while (input[char_idx] != ' ') {
        RETURN_NONE_IF_PAST_END;
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
            return std::optional<int>();
        }
        char_idx += 1;
    }
    char_idx += 1;

    RETURN_NONE_IF_PAST_END;
    if (input[char_idx] == '-') {
        set_en_passant_file(9);
        char_idx += 1;
    } else {
        if (input[char_idx] >= 'a' && input[char_idx] <= 'h') {
            set_en_passant_file(input[char_idx] - 97);
            // in ascii 'a' has the value 97, so we subtract 97 to get the index of the file
            char_idx += 2;
        } else {
            return std::optional<int>();
        }
    }
    char_idx += 1;
    // this sets the en passant file

    RETURN_NONE_IF_PAST_END;
    std::string halfmove_string;
    while (!std::isspace(input[char_idx])) {
        RETURN_NONE_IF_PAST_END;
        halfmove_string.push_back(input[char_idx]);
        char_idx += 1;
    }
    halfmove_clock = std::stoi(halfmove_string);
    char_idx += 1;

    RETURN_NONE_IF_PAST_END;
    std::string fullmove_string;
    while (!std::isspace(input[char_idx]) && input[char_idx] != '\0') {
        RETURN_NONE_IF_PAST_END;
        fullmove_string.push_back(input[char_idx]);
        char_idx += 1;
    }
    fullmove_counter = std::stoi(fullmove_string);

    return std::optional<int>(char_idx);
}

void ChessBoard::make_move(const Move to_make, MoveHistory& move_history) {
    Piece moved = this->pieces[to_make.get_src_square()];
    Piece at_target = this->pieces[to_make.get_dest_square()];
    auto to_add = MoveHistoryEntry(this->zobrist_key, checkers, pinned_pieces, to_make, at_target, this->en_passant_file, this->halfmove_clock,
                                   castling);
    move_history.push_move(to_add);
    this->zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
    halfmove_clock += 1;
    this->en_passant_file = 9;
    if (!to_make.is_null_move()) [[likely]] {
        const Side side = moved.get_side();
        this->pieces[to_make.get_src_square()] = 0;
        zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, to_make.get_src_square())];
        scores[static_cast<int>(side)] -= get_psqt_score(moved, to_make.get_src_square());

        // get the piece we're moving and clear the origin square

        if (to_make.is_capture() || moved.get_type() == PAWN) {
            halfmove_clock = 0;
        }

        clear_bit(this->bbs[moved.to_bitboard_idx()], to_make.get_src_square());
        if (at_target.get_value()) {
            // If there _was_ a piece there
            // we do this as en passant captures without a piece at the position
            clear_bit(this->bbs[at_target.to_bitboard_idx()], to_make.get_dest_square());
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(at_target, to_make.get_dest_square())];
            scores[static_cast<int>(enemy_side(side))] -= get_psqt_score(Piece(enemy_side(side), at_target.get_type()), to_make.get_dest_square());

            mg_phase -= mg_phase_vals[static_cast<int>(at_target.get_type()) - 1];
        }

        if (static_cast<int>(to_make.get_move_flags()) >= 8) {
            // Any value >= 8 is a promotion
            Piece promoted_piece = Piece(side, PieceTypes((static_cast<int>(to_make.get_move_flags()) & 0b0011) + 2));
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(promoted_piece, to_make.get_dest_square())];
            this->pieces[to_make.get_dest_square()] = promoted_piece;
            // promoted_piece += side;
            set_bit(this->bbs[promoted_piece.to_bitboard_idx()], to_make.get_dest_square());
            // This handles pawn promotions
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, promoted_piece.get_type()), to_make.get_dest_square());

            mg_phase += mg_phase_vals[static_cast<int>(promoted_piece.get_type()) - 1];
        } else {
            set_bit(this->bbs[moved.to_bitboard_idx()], to_make.get_dest_square());
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, to_make.get_dest_square())];
            this->pieces[to_make.get_dest_square()] = moved;
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, moved.get_type()), to_make.get_dest_square());
            // otherwise sets pieces if moved normally
        }

        if (to_make.get_move_flags() == MoveFlags::DOUBLE_PAWN_PUSH) [[unlikely]] {
            this->en_passant_file = to_make.get_dest_file();
            this->zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
        }
        // the en passant zobrist key for 9 is 0 so no need to XOR (would be a no-op)
        // set where the last en passant happened, else clear it

        if (to_make.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) [[unlikely]] {
            Side enemy = enemy_side(side);
            int enemy_pawn_idx = /*to_make.get_dest_square() - 8 + (16 * static_cast<int>(side));*/ get_position(to_make.get_src_rank(), to_make.get_dest_file());
            clear_bit(this->bbs[bb_idx<PAWN> + static_cast<int>(enemy)], enemy_pawn_idx);
            this->pieces[enemy_pawn_idx] = 0;
            this->zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(enemy, PAWN), enemy_pawn_idx)];
            scores[static_cast<int>(enemy)] -= get_psqt_score(Piece(enemy, PAWN), enemy_pawn_idx);
        }

        if (to_make.is_castling_move()) {
            const auto king_dest = to_make.get_dest_square();
            const auto rook_dest = king_dest + (to_make.get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1);
            const auto rook_origin = king_dest + (to_make.get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? 1 : -2);

            // we moved the king, now move the rook
            this->pieces[rook_dest] = this->pieces[rook_origin];
            this->pieces[rook_origin] = 0;

            clear_bit(this->bbs[bb_idx<ROOK> + static_cast<int>(side)], rook_origin);
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_origin)];
            scores[static_cast<int>(side)] -= get_psqt_score(Piece(side, ROOK), rook_origin);

            set_bit(this->bbs[bb_idx<ROOK> + static_cast<int>(side)], rook_dest);
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_dest)];
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, ROOK), rook_dest);
        }

        const auto offset_diff = castling_rights[to_make.get_src_square()] & castling_rights[to_make.get_dest_square()];
        const auto new_castling = castling & offset_diff;
        zobrist_key ^= castling_keys[new_castling ^ castling];
        castling = new_castling;
    }
    fullmove_counter += static_cast<int>(side_to_move);
    side_to_move = enemy_side(side_to_move);
    zobrist_key ^= ZobristKeys::SideToMove;
    recompute_blockers_and_checkers(side_to_move);
}

void ChessBoard::unmake_move(MoveHistory& move_history) {
    const auto unmake_info = move_history.pop_move();
    halfmove_clock = unmake_info.get_halfmove_clock();
    Piece moved = pieces[unmake_info.get_move().get_dest_square()];
    const Side moved_side = moved.get_side();
    if (!unmake_info.get_move().is_null_move()) {
        clear_bit(this->bbs[moved.to_bitboard_idx()], unmake_info.get_move().get_dest_square());
        scores[static_cast<int>(moved_side)] -= get_psqt_score(moved, unmake_info.get_move().get_dest_square());
        // this unsets the target piece
        pieces[unmake_info.get_move().get_dest_square()] = unmake_info.get_piece();

        if (static_cast<int>(unmake_info.get_move().get_move_flags()) >= 8) {
            // if it's a promotion
            Piece new_piece = Piece(moved.get_side(), PAWN);
            pieces[unmake_info.get_move().get_src_square()] = new_piece;
            set_bit(this->bbs[new_piece.to_bitboard_idx()], unmake_info.get_move().get_src_square());
            scores[static_cast<int>(moved_side)] += get_psqt_score(new_piece, unmake_info.get_move().get_src_square());

            mg_phase -= mg_phase_vals[static_cast<int>(moved.get_type()) - 1];
        } else {
            pieces[unmake_info.get_move().get_src_square()] = moved;
            set_bit(this->bbs[moved.to_bitboard_idx()], unmake_info.get_move().get_src_square());
            scores[static_cast<int>(moved_side)] += get_psqt_score(moved, unmake_info.get_move().get_src_square());
        }

        if (unmake_info.get_move().is_capture()) {
            if (unmake_info.get_move().get_move_flags() != MoveFlags::EN_PASSANT_CAPTURE) {
                // if it is _not_ an ep capture
                set_bit(this->bbs[unmake_info.get_piece().to_bitboard_idx()], unmake_info.get_move().get_dest_square());
                scores[static_cast<int>(enemy_side(moved_side))] += get_psqt_score(unmake_info.get_piece(), unmake_info.get_move().get_dest_square());

                mg_phase += mg_phase_vals[static_cast<int>(unmake_info.get_piece().get_type()) - 1];
            } else {
                const Side enemy = enemy_side(moved_side);
                const int enemy_pawn_idx = unmake_info.get_move().get_dest_square() - 8 + (16 * static_cast<int>(moved_side));
                this->pieces[enemy_pawn_idx] = Piece(enemy, PAWN);
                set_bit(this->bbs[bb_idx<PAWN> + static_cast<int>(enemy)], enemy_pawn_idx);
                scores[static_cast<int>(enemy_side(moved_side))] += get_psqt_score(Piece(enemy_side(moved_side), PAWN), enemy_pawn_idx);
            }
        }

        if (unmake_info.get_move().is_castling_move()) {
            const auto king_dest = unmake_info.get_move().get_dest_square();
            const auto rook_dest = king_dest + (unmake_info.get_move().get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1);
            const auto rook_origin = king_dest + (unmake_info.get_move().get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? 1 : -2);

            this->pieces[rook_origin] = this->pieces[rook_dest];
            this->pieces[rook_dest] = 0;

            clear_bit(this->bbs[bb_idx<ROOK> + static_cast<int>(moved_side)], rook_dest);
            scores[static_cast<int>(moved_side)] -= get_psqt_score(Piece(moved_side, ROOK), rook_dest);

            set_bit(this->bbs[bb_idx<ROOK> + static_cast<int>(moved_side)], rook_origin);
            scores[static_cast<int>(moved_side)] += get_psqt_score(Piece(moved_side, ROOK), rook_origin);
        }
    }

    set_en_passant_file(unmake_info.get_previous_en_passant_file());
    castling = unmake_info.get_castling();

    side_to_move = enemy_side(side_to_move);
    fullmove_counter -= static_cast<int>(side_to_move);
    zobrist_key = unmake_info.get_zobrist_key();
    checkers = unmake_info.get_checkers();
    pinned_pieces = unmake_info.get_pins();
}

void ChessBoard::recompute_blockers_and_checkers(const Side side) {
    const int king_idx = get_lsb(this->kings(side));
    const Side enemy = enemy_side(side);
    checkers = MoveGenerator::get_checkers(*this, side);

    pinned_pieces = 0;

    Bitboard potential_checks =
        ((MoveGenerator::generate_bishop_mm(0, king_idx) & (bishops(enemy) | queens(enemy))) |
         (MoveGenerator::generate_rook_mm(0, king_idx) & (rooks(enemy) | queens(enemy)))) ^
        checkers;
    const Bitboard check_blockers = occupancy() ^ potential_checks;
    // don't evaluate ones where we already check the king

    while (potential_checks) {
        const Bitboard line_to_king = MagicNumbers::ConnectingSquares[(64 * king_idx) + pop_lsb(potential_checks)];
        if (std::popcount(line_to_king & check_blockers) <= 1) {
            pinned_pieces |= line_to_king & check_blockers;
        }
    }
}

std::optional<Move> ChessBoard::generate_move_from_string(const std::string& s) const {
    if (s.size() != 4 && s.size() != 5) {
        return std::optional<Move>();
    }
    MoveFlags m = MoveFlags(0);
    if (s.size() == 5) {
        // if bit 8 is set it's a promotion
        switch (std::tolower(s.at(4))) {
        case 'r':
            m = MoveFlags::ROOK_PROMOTION;
            break;
        case 'n':
            m = MoveFlags::KNIGHT_PROMOTION;
            break;
        case 'q':
            m = MoveFlags::QUEEN_PROMOTION;
            break;
        case 'b':
            m = MoveFlags::BISHOP_PROMOTION;
            break;
        default:
            return std::optional<Move>();
        }
    }
    int start_square = get_position(s.at(1) - 49, s.at(0) - 97);
    int end_square = get_position(s.at(3) - 49, s.at(2) - 97);
    // Side move_side = this->get_side_to_move();
    PieceTypes moving_type = this->get_piece(start_square).get_type();
    if (moving_type == PAWN && (std::abs(start_square - end_square) == 7 || std::abs(start_square - end_square) == 9)) {
        // so a capture
        if (this->get_piece(end_square) == 0) {
            // if there's no piece at the target square it must be en passant
            m = MoveFlags::EN_PASSANT_CAPTURE;
        } else {
            m = MoveFlags(static_cast<int>(m) | static_cast<int>(MoveFlags::CAPTURE));
            // We or, not set, in case the promotion flags were set earlier
        }
    } else if (moving_type == PAWN && (std::abs(start_square - end_square) == 16)) {
        // A 16-space gap is a double pawn push
        m = MoveFlags::DOUBLE_PAWN_PUSH;
    } else if (moving_type == KING) {
        if (start_square - end_square == 2) {
            // this is a queenside castle
            m = MoveFlags::QUEENSIDE_CASTLE;
        } else if (end_square - start_square == 2) {
            m = MoveFlags::KINGSIDE_CASTLE;
        }
    } 
    if (this->get_piece(end_square) != 0) {
        m = MoveFlags(static_cast<int>(m) | static_cast<int>(MoveFlags::CAPTURE));
    }

    return std::optional<Move>(Move(m, end_square, start_square));
}

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs) {
    bool is_equal = true;
    for (int i = 0; i < 64; i++) {
        is_equal &= (lhs.get_piece(i) == rhs.get_piece(i));
    }

    for (int i = 0; i < 12; i++) {
        is_equal &= (lhs.get_bb(i) == rhs.get_bb(i));
        // this just makes it a bit nicer to use
    }

    is_equal &= (lhs.get_en_passant_file() == rhs.get_en_passant_file());

    for (Side s : {Side::WHITE, Side::BLACK}) {
        is_equal &= (lhs.get_queenside_castling(s) == rhs.get_queenside_castling(s));
        is_equal &= (lhs.get_kingside_castling(s) == rhs.get_kingside_castling(s));
    }

    return is_equal;
}