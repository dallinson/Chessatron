#include "chessboard.hpp"

#include <bit>
#include <cstring>
#include <string>
#include <utility>
#include <fmt/format.h>

#include "magic_numbers.hpp"
#include "magic_numbers/piece_square_tables.hpp"
#include "move_generator.hpp"
#include "uci_options.hpp"
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

void Position::set_piece(Piece piece, Square sq) {
    auto pos = sq_to_int(sq);
    piece_bbs[static_cast<int>(piece.type()) - 1] |= sq;
    side_bbs[static_cast<int>(piece.side())] |= sq;
    piece_mb[pos] = piece;
    _zobrist_key ^= ZobristKeys::PositionKeys[calculate_zobrist_key(piece, pos)];
    if (piece.type() == PieceTypes::PAWN) {
        _pawn_hash ^= ZobristKeys::PositionKeys[calculate_zobrist_key(piece, pos)];
    } else {
        _side_non_pawn_hashes[static_cast<int>(piece.side())] ^= ZobristKeys::PositionKeys[calculate_zobrist_key(piece, pos)];
    }

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
    castling_rights = 0;
    castling_files.fill(9);
    castling_rights_per_square.fill(15);

    _zobrist_key = ZobristKeys::SideToMove;
    _pawn_hash = 0;
    _side_non_pawn_hashes = { 0, 0 };

    scores = {0};
    // Only the white side to move key should be set
    mg_phase = 0;
}

void Position::print_board() const {
    static const char* piece_str = ".PNBRQK..pnbrqk.";
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            fmt::print("{}", piece_str[piece_at(get_position(rank, file)).val()]);
        }
        fmt::println("");
    }
    std::string castle_string = "";
    if (get_kingside_castling(Side::WHITE)) {
        castle_string.push_back('K');
    }
    if (get_kingside_castling(Side::WHITE)) {
        castle_string.push_back('Q');
    }
    if (get_kingside_castling(Side::BLACK)) {
        castle_string.push_back('k');
    }
    if (get_kingside_castling(Side::BLACK)) {
        castle_string.push_back('q');
    }
    if (castle_string.length() == 0) {
        castle_string = "-";
    }
    fmt::println("Side to move: {}", stm() == Side::WHITE ? "WHITE" : "BLACK");
    fmt::println("Castling: {}", castle_string);
    const char ep_chr = en_passant_file != 9 ? (static_cast<char>(en_passant_file) + 'a') : '-';
    fmt::println("En passant file: {}", ep_chr);
    fmt::println("Halfmove counter: {}", halfmove_clock);
    fmt::println("Fullmove counter: {}", fullmove_counter);
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
    int fle = 0;
    int char_idx = 0;
    // set up the board
    while (input[char_idx] != ' ') {
        if (input[char_idx] == '/') {
            fle = 0;
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
                fle += (current - 48);
                char_idx += 1;
                continue;

            default:
                return std::optional<int>();
            }
            Piece piece = Piece(piece_side, piece_value);
            set_piece(piece, get_position(rank, fle));
            // zobrist_key ^= ZobristKeys::PositionKeys[(piece * 64) + get_position(rank, file)];
            fle += 1;
            if (fle > 8) {
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
    bool is_dfrc = false;
    while (input[char_idx] != ' ') {
        RETURN_NONE_IF_PAST_END;
        const auto chr = input[char_idx];
        if (chr == 'K' || chr == 'k') {
            const auto rook_fle = find_outer_rook(chr == 'K' ? Side::WHITE : Side::BLACK, true);
            is_dfrc |= (rook_fle != 7);
            is_dfrc |= file(kings(chr == 'K' ? Side::WHITE : Side::BLACK).lsb()) != 4; // if the king file != 4 then we are definitely not in normal chess
            set_castling_from_fen((chr - ('K' - 'A')) + rook_fle);
            // this is kinda hard to get
            // 'K' - 'A' is the offset between these two chars
            // We subtract this from chr to get the case-matched 'a'
            // Then we add the rook file
        } else if (chr == 'Q' || chr == 'q') {
            const auto rook_fle = find_outer_rook(chr == 'Q' ? Side::WHITE : Side::BLACK, true);
            is_dfrc |= (rook_fle != 7);
            is_dfrc |= file(kings(chr == 'K' ? Side::WHITE : Side::BLACK).lsb()) != 4;
            set_castling_from_fen((chr - ('Q' - 'A')) + rook_fle);
        } else if (chr != '-') {
            is_dfrc |= true;
            set_castling_from_fen(chr);
        }
        char_idx += 1;
    }
    uci_options()["UCI_Chess960"].set_value(is_dfrc ? "true" : "false");
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

auto Position::find_outer_rook(const Side side, const bool is_kingside) const -> u8 {
    const auto king_sq = kings(side).lsb();
    const auto rook = Piece(side, PieceTypes::ROOK);
    const auto rnk = side == Side::WHITE ? 0 : 7;
    if (is_kingside) {
        for (u8 fle = 7; fle > file(king_sq); fle--) {
            if (piece_at(square(rnk, fle)) == rook) {
                return fle;
            }
        }
        std::unreachable();
    } else {
        for (u8 fle = 0; fle < file(king_sq); fle++) {
            if (piece_at(square(rnk, fle)) == rook) {
                return fle;
            }
        }
        std::unreachable();
    }
}

void Position::set_castling_from_fen(char _chr) {
    const auto side = static_cast<u8>(_chr) >= static_cast<u8>('a') ? Side::BLACK : Side::WHITE;
    // Black pieces/castling is lowercase
    if (side == Side::BLACK) {
        _chr -= ('a' - 'A'); // Convert to uppercase
    }
    const auto chr = _chr;
    const auto target_file = chr - 'A'; // We treat the A-file as 0
    const auto king_sq = kings(side).lsb();
    const auto king_file = file(king_sq);
    const auto is_kingside = target_file > king_file; // We can never castle TO the king's file
    if (is_kingside) {
        set_kingside_castling(side, true);
    } else {
        set_queenside_castling(side, true);
    }
    castling_files[castling_idx(side, is_kingside)] = target_file;
    const auto rank = ((side == Side::WHITE) ? 0 : 7);

    const auto rook_sq = square(rank, target_file);
    const u8 castling_mask = ~(1 << castling_idx(side, is_kingside));

    castling_rights_per_square[static_cast<i32>(rook_sq)] &= castling_mask;
    castling_rights_per_square[static_cast<i32>(king_sq)] &= castling_mask;
}

auto Position::makemove_remove_piece(const Square sq) -> void {
    assert(piece_at(sq).val() != 0);
    const auto p = piece_at(sq);

    // First, we clear the bbs
    piece_bbs[bb_idx(p.type())] &= ~Bitboard(sq);
    side_bbs[static_cast<i32>(p.side())] &= ~Bitboard(sq);
    // Then clear the mailbox
    piece_mb[static_cast<i32>(sq)] = 0;

    // Now we do the zobrist changes!
    _zobrist_key ^= get_zkey(p, sq);
    if (p.type() == PAWN) {
        _pawn_hash ^= get_zkey(p, sq);
    } else {
        _side_non_pawn_hashes[static_cast<i32>(p.side())] ^= get_zkey(p, sq);
    }
    scores[static_cast<i32>(p.side())] -= get_psqt_score(p, sq);
    // And finally adjust the mg phase
    mg_phase -= mg_phase_vals[static_cast<i32>(p.type()) - 1];
}

auto Position::makemove_add_piece(const Piece p, const Square sq) -> void {
    assert(piece_at(sq).val() == 0);

    // Set the BBs
    piece_bbs[bb_idx(p.type())] |= sq;
    side_bbs[static_cast<i32>(p.side())] |= sq;
    // Set the mailbox
    piece_mb[static_cast<i32>(sq)] = p;

    _zobrist_key ^= get_zkey(p, sq);
    if (p.type() == PAWN) {
        _pawn_hash ^= get_zkey(p, sq);
    } else {
        _side_non_pawn_hashes[static_cast<i32>(p.side())] ^= get_zkey(p, sq);
    }
    scores[static_cast<i32>(p.side())] += get_psqt_score(p, sq);
    // And finally adjust the mg phase
    mg_phase += mg_phase_vals[static_cast<i32>(p.type()) - 1];
}

Position::Position(const Position& origin, const Move to_make) {
    assert(MoveGenerator::is_move_legal(origin, to_make));

    *this = origin;
    this->_zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
    halfmove_clock += 1;
    this->en_passant_file = 9;

    if (!to_make.is_null_move()) [[likely]] {
        const auto src_sq = to_make.src_sq();
        const auto dst_sq = [&]() {
            if (!to_make.is_castling_move()) {
                return to_make.dst_sq();
            } else {
                const auto target_rnk = to_make.dst_rnk();
                const auto target_fle = to_make.flags() == MoveFlags::KINGSIDE_CASTLE ? 6 : 2;
                return square(target_rnk, target_fle);
            }
        }();
        const auto moved = piece_at(src_sq);
        const auto side = moved.side();

        if (to_make.is_capture() || moved.type() == PAWN) {
            halfmove_clock = 0;
        }

        makemove_remove_piece(src_sq);

        if (to_make.is_castling_move()) {
            makemove_remove_piece(to_make.dst_sq());
        }

        if (to_make.is_capture()) {
            if (to_make.flags() != MoveFlags::EN_PASSANT_CAPTURE) {
                makemove_remove_piece(to_make.dst_sq());
            } else {
                const auto ep_square = square(to_make.src_rnk(), to_make.dst_fle());
                makemove_remove_piece(ep_square);
            }
        }

        // Now readd the piece
        if (to_make.is_promotion()) {
            const auto promoted_piece = Piece(side, to_make.promo_type());
            makemove_add_piece(promoted_piece, dst_sq);
        } else {
            makemove_add_piece(moved, dst_sq);
        }

        if (to_make.flags() == MoveFlags::DOUBLE_PAWN_PUSH) [[unlikely]] {
            this->en_passant_file = to_make.dst_fle();
            this->_zobrist_key ^= ZobristKeys::EnPassantKeys[en_passant_file];
        }
        // the en passant zobrist key for 9 is 0 so no need to XOR (would be a no-op)
        // set where the last en passant happened, else clear it

        if (to_make.is_castling_move()) {
            makemove_add_piece(Piece(side, PieceTypes::ROOK), dst_sq + (to_make.flags() == MoveFlags::KINGSIDE_CASTLE ? -1 : 1));
        }

        const auto offset_diff = castling_rights_per_square[sq_to_int(to_make.src_sq())] & castling_rights_per_square[sq_to_int(dst_sq)];
        const auto new_castling = castling_rights & offset_diff;
        _zobrist_key ^= castling_keys[new_castling ^ castling_rights];
        castling_rights = new_castling;
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
    if (at_target.val()) {
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
    auto end_sq = get_position(s.at(3) - 49, s.at(2) - 97);
    // Side move_side = this->stm();
    const auto moving_type = this->piece_at(start_sq).type();
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
        const auto side = this->piece_at(start_sq).side();
        const auto end_pc = this->piece_at(end_sq);
        const auto rook_castle = end_pc.side() == side && end_pc.type() == PieceTypes::ROOK;
        const auto offset_castle = std::abs(offset) == 2;
        if ((rook_castle && end_sq < start_sq) || (offset_castle && offset == 2)) {
            // this is a queenside castle
            m = MoveFlags::QUEENSIDE_CASTLE;
            end_sq = square(rank(end_sq), find_outer_rook(side, false));
        } else if ((rook_castle && end_sq > start_sq) || (offset_castle && offset == -2)) {
            m = MoveFlags::KINGSIDE_CASTLE;
            end_sq = square(rank(end_sq), find_outer_rook(side, true));
        }
    }
    if (occupancy()[end_sq] && !(m == MoveFlags::QUEENSIDE_CASTLE || m == MoveFlags::KINGSIDE_CASTLE)) {
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