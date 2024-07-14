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

void Position::set_piece(Piece piece, Square sq) {
    auto pos = sq_to_int(sq);
    piece_bbs[static_cast<int>(piece.type()) - 1] |= sq;
    side_bbs[static_cast<int>(piece.side())] |= sq;
    piece_mb[pos] = piece;
    _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(piece, pos)];
    if (piece.type() == PieceTypes::PAWN) _pawn_hash ^= ZobristKeys::PositionKeys[calculate_zobrist_key(piece, pos)];

    const auto piece_side = piece.side();
    const auto piece_type = piece.type();
    if (piece_side == Side::WHITE) {
        pos ^= 0b00111000;
    }

    scores[static_cast<int>(piece_side)] += Tables[static_cast<int>(piece_type) - 1][pos];

    mg_phase += mg_phase_vals[static_cast<int>(piece_type) - 1];
}

void Position::clear_board() {
    for (int i = 0; i < 6; i++) {
        piece_bbs[i] = 0;
    }
    side_bbs[0] = 0;
    side_bbs[1] = 0;
    piece_mb.fill(0);

    side_to_move = Side::WHITE;
    en_passant_file = 9;
    castling = 0;

    _zobrist_key = ZobristKeys::SideToMove;
    _pawn_hash = 0;

    scores = {0};
    // Only the white side to move key should be set
    mg_phase = 0;
}

void Position::print_board() const {
    static const char* piece_str = ".PNBRQK..pnbrqk.";
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            printf("%c", piece_str[piece_at(get_position(rank, file)).get_value()]);
        }
        printf("\n");
    }
}

#define RETURN_NONE_IF_PAST_END                                                                                                                      \
    if ((size_t) char_idx >= (input).size()) {                                                                                                       \
        return std::optional<int>();                                                                                                                 \
    }

std::optional<int> Position::set_from_fen(const std::string input) {
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
        _zobrist_key ^= ZobristKeys::SideToMove;
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

Position::Position(const Position& origin, const Move to_make) {
    assert(MoveGenerator::is_move_legal(origin, to_make));

    *this = origin;
    const auto src_sq = to_make.src_sq();
    const auto dest_sq = to_make.dst_sq();
    const auto moved = piece_at(src_sq);
    Piece at_target = static_cast<Piece>(0);
    if (occupancy()[dest_sq]) {
        at_target = piece_at(dest_sq);
    }
    this->_zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
    halfmove_clock += 1;
    this->en_passant_file = 9;
    if (!to_make.is_null_move()) [[likely]] {
        const Side side = moved.side();
        _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, src_sq)];
        if (moved.type() == PAWN) _pawn_hash ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, src_sq)];
        scores[static_cast<int>(side)] -= get_psqt_score(moved, src_sq);

        // get the piece we're moving and clear the origin square

        if (to_make.is_capture() || moved.type() == PAWN) {
            halfmove_clock = 0;
        }

        piece_bbs[static_cast<int>(moved.type()) - 1] &= ~Bitboard(src_sq);
        side_bbs[static_cast<int>(moved.side())] &= ~Bitboard(src_sq);
        piece_mb[sq_to_int(src_sq)] = 0;
        if (at_target.get_value()) {
            // If there _was_ a piece there
            // we do this as en passant captures without a piece at the position
            piece_bbs[static_cast<int>(at_target.type()) - 1] &= ~Bitboard(dest_sq);
            side_bbs[static_cast<int>(at_target.side())] &= ~Bitboard(dest_sq);

            _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(at_target, dest_sq)];
            if (at_target.type() == PAWN) _pawn_hash ^= ZobristKeys::PositionKeys[calculate_zobrist_key(at_target, dest_sq)];
            scores[static_cast<int>(enemy_side(side))] -= get_psqt_score(Piece(enemy_side(side), at_target.type()), dest_sq);

            mg_phase -= mg_phase_vals[static_cast<int>(at_target.type()) - 1];
        }

        if (static_cast<int>(to_make.flags()) >= 8) {
            // Any value >= 8 is a promotion
            Piece promoted_piece = Piece(side, PieceTypes((static_cast<int>(to_make.flags()) & 0b0011) + 2));
            _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(promoted_piece, dest_sq)];
            // promoted_piece += side;
            this->piece_bbs[static_cast<int>(promoted_piece.type()) - 1] |= dest_sq;
            piece_mb[sq_to_int(dest_sq)] = promoted_piece;
            // This handles pawn promotions
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, promoted_piece.type()), dest_sq);

            mg_phase += mg_phase_vals[static_cast<int>(promoted_piece.type()) - 1];
        } else {
            this->piece_bbs[static_cast<int>(moved.type()) - 1] |= dest_sq;
            piece_mb[sq_to_int(dest_sq)] = moved;
            _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, dest_sq)];
            if (moved.type() == PAWN) _pawn_hash ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, to_make.dst_sq())];
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, moved.type()), dest_sq);
            // otherwise sets pieces if moved normally
        }
        this->side_bbs[static_cast<int>(moved.side())] |= dest_sq;

        if (to_make.flags() == MoveFlags::DOUBLE_PAWN_PUSH) [[unlikely]] {
            this->en_passant_file = to_make.dst_fle();
            this->_zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
        }
        // the en passant zobrist key for 9 is 0 so no need to XOR (would be a no-op)
        // set where the last en passant happened, else clear it

        if (to_make.flags() == MoveFlags::EN_PASSANT_CAPTURE) [[unlikely]] {
            Side enemy = enemy_side(side);
            const auto enemy_pawn_idx = get_position(to_make.src_rnk(), to_make.dst_fle());
            this->piece_bbs[bb_idx<PAWN>] &= ~Bitboard(enemy_pawn_idx);
            this->side_bbs[static_cast<int>(enemy_side(side))] &= ~Bitboard(enemy_pawn_idx);
            piece_mb[sq_to_int(enemy_pawn_idx)] = 0;
            this->_zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(enemy, PAWN), enemy_pawn_idx)];
            _pawn_hash ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(enemy, PAWN), enemy_pawn_idx)];
            scores[static_cast<int>(enemy)] -= get_psqt_score(Piece(enemy, PAWN), enemy_pawn_idx);
        }

        if (to_make.is_castling_move()) {
            const auto king_dest = dest_sq;
            const auto rook_dest = king_dest + (to_make.flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1);
            const auto rook_origin = king_dest + (to_make.flags() == MoveFlags::KINGSIDE_CASTLE ? 1 : -2);

            // we moved the king, now move the rook

            this->piece_bbs[bb_idx<ROOK>] &= ~Bitboard(rook_origin);
            this->side_bbs[static_cast<int>(side)] &= ~Bitboard(rook_origin);
            piece_mb[sq_to_int(rook_origin)] = 0;
            _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_origin)];
            scores[static_cast<int>(side)] -= get_psqt_score(Piece(side, ROOK), rook_origin);

            this->piece_bbs[bb_idx<ROOK>] |= rook_dest;
            this->side_bbs[static_cast<int>(side)] |= rook_dest;
            piece_mb[sq_to_int(rook_dest)] = Piece(side, PieceTypes::ROOK);
            _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_dest)];
            scores[static_cast<int>(side)] += get_psqt_score(Piece(side, ROOK), rook_dest);
        }

        const auto offset_diff = castling_rights[sq_to_int(to_make.src_sq())] & castling_rights[sq_to_int(dest_sq)];
        const auto new_castling = castling & offset_diff;
        _zobrist_key ^= castling_keys[new_castling ^ castling];
        castling = new_castling;
    }
    fullmove_counter += static_cast<int>(side_to_move);
    side_to_move = enemy_side(side_to_move);
    _zobrist_key ^= ZobristKeys::SideToMove;
    recompute_blockers_and_checkers(side_to_move);
}

Position& Position::make_move(const Move to_make, BoardHistory& history) const { return history.push_board(Position(*this, to_make), to_make); }

void Position::recompute_blockers_and_checkers(const Side side) {
    const auto king_idx = this->kings(side).lsb();
    const Side enemy = enemy_side(side);
    _checkers = MoveGenerator::get_checkers(*this, side);

    _pinned_pieces = 0;

    Bitboard potential_checks = ((MoveGenerator::generate_bishop_mm(0, king_idx) & (bishops(enemy) | queens(enemy)))
                                 | (MoveGenerator::generate_rook_mm(0, king_idx) & (rooks(enemy) | queens(enemy))))
                                ^ _checkers;
    const Bitboard check_blockers = occupancy() ^ potential_checks;
    // don't evaluate ones where we already check the king

    while (!potential_checks.empty()) {
        const Bitboard line_to_king = MagicNumbers::ConnectingSquares[sq_to_int(king_idx)][sq_to_int(potential_checks.pop_lsb())];
        if ((line_to_king & check_blockers).popcnt() <= 1) {
            _pinned_pieces |= line_to_king & check_blockers;
        }
    }
}

ZobristKey Position::key_after(const Move move) const {
    assert(!move.is_null_move());
    auto to_return = _zobrist_key;
    const auto src_sq = move.src_sq();
    const auto dest_sq = move.dst_sq();
    const auto moved = piece_at(src_sq);
    const Side side = moved.side();

    Piece at_target = static_cast<Piece>(0);
    if (occupancy()[dest_sq]) {
        at_target = piece_at(dest_sq);
    }

    to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, src_sq)];
    if (at_target.get_value()) {
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(at_target, dest_sq)];
    }

    if (move.is_promotion()) {
        Piece promoted_piece = Piece(side, PieceTypes((static_cast<int>(move.flags()) & 0b0011) + 2));
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(promoted_piece, dest_sq)];
    } else {
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(moved, dest_sq)];
    }

    if (move.flags() == MoveFlags::DOUBLE_PAWN_PUSH) {
        to_return ^= ZobristKeys::EnPassantKeys[move.dst_fle()];
    }

    if (move.flags() == MoveFlags::EN_PASSANT_CAPTURE) {
        const auto enemy = enemy_side(side);
        const auto enemy_pawn_idx = get_position(move.src_rnk(), move.dst_fle());
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(enemy, PAWN), enemy_pawn_idx)];
    }

    if (move.is_castling_move()) {
        const auto king_dest = move.dst_sq();
        const auto rook_dest = king_dest + (move.flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1);
        const auto rook_origin = king_dest + (move.flags() == MoveFlags::KINGSIDE_CASTLE ? 1 : -2);

        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_origin)];
        to_return ^= ZobristKeys::PositionKeys[calculate_zobrist_key(Piece(side, ROOK), rook_dest)];
    }

    return to_return ^ ZobristKeys::SideToMove;
}

std::optional<Move> Position::generate_move_from_string(const std::string& s) const {
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
    const auto start_sq = get_position(s.at(1) - 49, s.at(0) - 97);
    const auto end_sq = get_position(s.at(3) - 49, s.at(2) - 97);
    // Side move_side = this->stm();
    PieceTypes moving_type = this->piece_at(start_sq).type();
    const auto offset = sq_to_int(start_sq) - sq_to_int(end_sq);
    if (moving_type == PAWN && (std::abs(offset) == 7 || std::abs(offset) == 9)) {
        // so a capture
        if (this->piece_at(end_sq) == 0) {
            // if there's no piece at the target square it must be en passant
            m = MoveFlags::EN_PASSANT_CAPTURE;
        } else {
            m = MoveFlags(static_cast<int>(m) | static_cast<int>(MoveFlags::CAPTURE));
            // We or, not set, in case the promotion flags were set earlier
        }
    } else if (moving_type == PAWN && (std::abs(offset) == 16)) {
        // A 16-space gap is a double pawn push
        m = MoveFlags::DOUBLE_PAWN_PUSH;
    } else if (moving_type == KING) {
        if (offset == 2) {
            // this is a queenside castle
            m = MoveFlags::QUEENSIDE_CASTLE;
        } else if (offset == -2) {
            m = MoveFlags::KINGSIDE_CASTLE;
        }
    }
    if (occupancy()[end_sq]) {
        m = MoveFlags(static_cast<int>(m) | static_cast<int>(MoveFlags::CAPTURE));
    }

    return std::optional<Move>(Move(m, end_sq, start_sq));
}

bool operator==(const Position& lhs, const Position& rhs) {
    bool is_equal = true;
    for (int pt = 0; pt < 6; pt++) {
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