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

constexpr static std::array<uint8_t, 6> mg_phase_vals = {0, 1, 1, 2, 4, 0};
const static std::array<ZobristKey, 16> castling_keys = {
    0,                                                           // 0000
    ZobristKeys::CastlingKeys[0],                                // 0001
    ZobristKeys::CastlingKeys[1],                                // 0010
    ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 0011

    ZobristKeys::CastlingKeys[2],                                                               // 0100
    ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0],                                // 0101
    ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[1],                                // 0110
    ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 0111

    ZobristKeys::CastlingKeys[3],                                                               // 1000
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[0],                                // 1001
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[1],                                // 1010
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 1011

    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2],                                                               // 1100
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0],                                // 1101
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[1],                                // 1110
    ZobristKeys::CastlingKeys[3] ^ ZobristKeys::CastlingKeys[2] ^ ZobristKeys::CastlingKeys[0] ^ ZobristKeys::CastlingKeys[1], // 1111
};
// Bit 0 is white kingside castling
// Bit 1 is black kingside castling
// Bit 2 is white queenside castling
// Bit 3 is black kingside castline

constexpr int castling_rights[64] = {0b1011, 15, 15, 15, 0b1010, 15, 15, 0b1110, 15, 15, 15, 15, 15,     15, 15, 15, 15,     15, 15, 15,    15, 15,
                                     15,     15, 15, 15, 15,     15, 15, 15,     15, 15, 15, 15, 15,     15, 15, 15, 15,     15, 15, 15,    15, 15,
                                     15,     15, 15, 15, 15,     15, 15, 15,     15, 15, 15, 15, 0b0111, 15, 15, 15, 0b0101, 15, 15, 0b1101};

void ChessBoard::set_piece(Piece piece, uint8_t pos) {
    set_bit(piece_bbs[static_cast<int>(piece.get_type()) - 1], pos);
    set_bit(side_bbs[static_cast<int>(piece.get_side())], pos);
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
    for (int i = 0; i < 6; i++) {
        piece_bbs[i] = 0;
    }
    side_bbs[0] = 0;
    side_bbs[1] = 0;

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
            printf("%c", piece_str[piece_at((rank * 8) + file).get_value()]);
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

ChessBoard::ChessBoard(const ChessBoard& origin, const Move to_make) {
    *this = origin;
    const auto src_sq = to_make.src_sq();
    const auto dest_sq = to_make.dst_sq();
    const auto moved = piece_at(src_sq);
    Piece at_target = static_cast<Piece>(0);
    if (get_bit(occupancy(), dest_sq) != 0) {
        at_target = piece_at(dest_sq);
    }
    this->zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
    halfmove_clock += 1;
    this->en_passant_file = 9;
    if (!to_make.is_null_move()) [[likely]] {
        const Side side = moved.get_side();
        zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, src_sq)];
        scores[static_cast<int>(side)] -= get_psqt_score(moved, src_sq);

        // get the piece we're moving and clear the origin square

        if (to_make.is_capture() || moved.get_type() == PAWN) {
            halfmove_clock = 0;
        }

        clear_bit(piece_bbs[static_cast<int>(moved.get_type()) - 1], src_sq);
        clear_bit(side_bbs[static_cast<int>(moved.get_side())], src_sq);
        if (at_target.get_value()) {
            // If there _was_ a piece there
            // we do this as en passant captures without a piece at the position
            clear_bit(piece_bbs[static_cast<int>(at_target.get_type()) - 1], dest_sq);
            clear_bit(side_bbs[static_cast<int>(at_target.get_side())], dest_sq);

            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(at_target, to_make.dst_sq())];
            scores[static_cast<int>(enemy_side(side))] -= get_psqt_score(Piece(enemy_side(side), at_target.get_type()), to_make.dst_sq());

            mg_phase -= mg_phase_vals[static_cast<int>(at_target.get_type()) - 1];
        }

        if (static_cast<int>(to_make.get_move_flags()) >= 8) {
            // Any value >= 8 is a promotion
            Piece promoted_piece = Piece(side, PieceTypes((static_cast<int>(to_make.get_move_flags()) & 0b0011) + 2));
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(promoted_piece, dest_sq)];
            // promoted_piece += side;
            set_bit(this->piece_bbs[static_cast<int>(promoted_piece.get_type()) - 1], dest_sq);
            // This handles pawn promotions
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, promoted_piece.get_type()), dest_sq);

            mg_phase += mg_phase_vals[static_cast<int>(promoted_piece.get_type()) - 1];
        } else {
            set_bit(this->piece_bbs[static_cast<int>(moved.get_type()) - 1], dest_sq);
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, to_make.dst_sq())];
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, moved.get_type()), dest_sq);
            // otherwise sets pieces if moved normally
        }
        set_bit(this->side_bbs[static_cast<int>(moved.get_side())], dest_sq);

        if (to_make.get_move_flags() == MoveFlags::DOUBLE_PAWN_PUSH) [[unlikely]] {
            this->en_passant_file = to_make.dst_fle();
            this->zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
        }
        // the en passant zobrist key for 9 is 0 so no need to XOR (would be a no-op)
        // set where the last en passant happened, else clear it

        if (to_make.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) [[unlikely]] {
            Side enemy = enemy_side(side);
            int enemy_pawn_idx = get_position(to_make.src_rnk(), to_make.dst_fle());
            clear_bit(this->piece_bbs[bb_idx<PAWN>], enemy_pawn_idx);
            clear_bit(this->side_bbs[static_cast<int>(enemy_side(side))], enemy_pawn_idx);
            this->zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(enemy, PAWN), enemy_pawn_idx)];
            scores[static_cast<int>(enemy)] -= get_psqt_score(Piece(enemy, PAWN), enemy_pawn_idx);
        }

        if (to_make.is_castling_move()) {
            const auto king_dest = to_make.dst_sq();
            const auto rook_dest = king_dest + (to_make.get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1);
            const auto rook_origin = king_dest + (to_make.get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? 1 : -2);

            // we moved the king, now move the rook

            clear_bit(this->piece_bbs[bb_idx<ROOK>], rook_origin);
            clear_bit(this->side_bbs[static_cast<int>(side)], rook_origin);
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_origin)];
            scores[static_cast<int>(side)] -= get_psqt_score(Piece(side, ROOK), rook_origin);

            set_bit(this->piece_bbs[bb_idx<ROOK>], rook_dest);
            set_bit(this->side_bbs[static_cast<int>(side)], rook_dest);
            zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_dest)];
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, ROOK), rook_dest);
        }

        const auto offset_diff = castling_rights[to_make.src_sq()] & castling_rights[to_make.dst_sq()];
        const auto new_castling = castling & offset_diff;
        zobrist_key ^= castling_keys[new_castling ^ castling];
        castling = new_castling;
    }
    fullmove_counter += static_cast<int>(side_to_move);
    side_to_move = enemy_side(side_to_move);
    zobrist_key ^= ZobristKeys::SideToMove;
    recompute_blockers_and_checkers(side_to_move);
}

ChessBoard& ChessBoard::make_move(const Move to_make, BoardHistory& history) const { return history.push_board(ChessBoard(*this, to_make)); }

void ChessBoard::recompute_blockers_and_checkers(const Side side) {
    const int king_idx = get_lsb(this->kings(side));
    const Side enemy = enemy_side(side);
    _checkers = MoveGenerator::get_checkers(*this, side);

    _pinned_pieces = 0;

    Bitboard potential_checks = ((MoveGenerator::generate_bishop_mm(0, king_idx) & (bishops(enemy) | queens(enemy)))
                                 | (MoveGenerator::generate_rook_mm(0, king_idx) & (rooks(enemy) | queens(enemy))))
                                ^ _checkers;
    const Bitboard check_blockers = occupancy() ^ potential_checks;
    // don't evaluate ones where we already check the king

    while (potential_checks) {
        const Bitboard line_to_king = MagicNumbers::ConnectingSquares[(64 * king_idx) + pop_lsb(potential_checks)];
        if (std::popcount(line_to_king & check_blockers) <= 1) {
            _pinned_pieces |= line_to_king & check_blockers;
        }
    }
}

ZobristKey ChessBoard::key_after(const Move move) const {
    assert(!move.is_null_move());
    auto to_return = zobrist_key;
    const auto src_sq = move.src_sq();
    const auto dest_sq = move.dst_sq();
    const auto moved = piece_at(src_sq);
    const Side side = moved.get_side();

    Piece at_target = static_cast<Piece>(0);
    if (get_bit(occupancy(), dest_sq) != 0) {
        at_target = piece_at(dest_sq);
    }

    to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, src_sq)];
    if (at_target.get_value()) {
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(at_target, dest_sq)];
    }

    if (move.is_promotion()) {
        Piece promoted_piece = Piece(side, PieceTypes((static_cast<int>(move.get_move_flags()) & 0b0011) + 2));
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(promoted_piece, dest_sq)];
    } else {
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, dest_sq)];
    }

    if (move.get_move_flags() == MoveFlags::DOUBLE_PAWN_PUSH) {
        to_return ^= ZobristKeys::EnPassantKeys[move.dst_fle()];
    }

    if (move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        const auto enemy = enemy_side(side);
        int enemy_pawn_idx = get_position(move.src_rnk(), move.dst_fle());
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(enemy, PAWN), enemy_pawn_idx)];
    }

    if (move.is_castling_move()) {
        const auto king_dest = move.dst_sq();
        const auto rook_dest = king_dest + (move.get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1);
        const auto rook_origin = king_dest + (move.get_move_flags() == MoveFlags::KINGSIDE_CASTLE ? 1 : -2);

        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_origin)];
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_dest)];
    }

    return to_return ^ ZobristKeys::SideToMove;
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
    int start_sq = get_position(s.at(1) - 49, s.at(0) - 97);
    int end_sq = get_position(s.at(3) - 49, s.at(2) - 97);
    // Side move_side = this->get_side_to_move();
    PieceTypes moving_type = this->piece_at(start_sq).get_type();
    if (moving_type == PAWN && (std::abs(start_sq - end_sq) == 7 || std::abs(start_sq - end_sq) == 9)) {
        // so a capture
        if (this->piece_at(end_sq) == 0) {
            // if there's no piece at the target square it must be en passant
            m = MoveFlags::EN_PASSANT_CAPTURE;
        } else {
            m = MoveFlags(static_cast<int>(m) | static_cast<int>(MoveFlags::CAPTURE));
            // We or, not set, in case the promotion flags were set earlier
        }
    } else if (moving_type == PAWN && (std::abs(start_sq - end_sq) == 16)) {
        // A 16-space gap is a double pawn push
        m = MoveFlags::DOUBLE_PAWN_PUSH;
    } else if (moving_type == KING) {
        if (start_sq - end_sq == 2) {
            // this is a queenside castle
            m = MoveFlags::QUEENSIDE_CASTLE;
        } else if (end_sq - start_sq == 2) {
            m = MoveFlags::KINGSIDE_CASTLE;
        }
    }
    if (get_bit(occupancy(), end_sq) != 0) {
        m = MoveFlags(static_cast<int>(m) | static_cast<int>(MoveFlags::CAPTURE));
    }

    return std::optional<Move>(Move(m, end_sq, start_sq));
}

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs) {
    bool is_equal = true;
    for (int pt = 1; pt <= 6; pt++) {
        for (int sd = 0; sd < 2; sd++) {
            is_equal &= (lhs.get_bb(pt, sd) == rhs.get_bb(pt, sd));
        }
    }

    is_equal &= (lhs.get_en_passant_file() == rhs.get_en_passant_file());

    for (Side s : {Side::WHITE, Side::BLACK}) {
        is_equal &= (lhs.get_queenside_castling(s) == rhs.get_queenside_castling(s));
        is_equal &= (lhs.get_kingside_castling(s) == rhs.get_kingside_castling(s));
    }

    return is_equal;
}