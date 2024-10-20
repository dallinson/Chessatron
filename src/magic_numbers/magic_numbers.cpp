#include "../magic_numbers.hpp"

#include <cmath>
#include <concepts>

#include "../utils.hpp"

template <typename T> constexpr T constexpr_sign(const T x) { return ((x) > 0) - ((x) < 0); };
// clever way of doing a sign function, from https://stackoverflow.com/a/1903975
template <typename T> constexpr T constexpr_abs(const T x) { return (((x) < 0) ? -(x) : (x)); };

template <typename T> constexpr T constexpr_min(const T a, const T b) { return ((a) < (b)) ? (a) : (b); };
template <typename T> constexpr T constexpr_max(const T a, const T b) { return ((a) > (b)) ? (a) : (b); };

/**
 * @brief This generates the squares between a first square and a second square for each pair of squares, as used in the pin detection algorithm used
 * in Stockfish and reimplemented here.  If the two spaces have a line between them, the attacking/pinning piece is a slider and the attack can be
 * stopped by moving into any of these spaces or capturing the attacking piece.  If not, the attack can only be stopped by capturing the attacking
 * piece
 *
 * @return consteval
 */
consteval MDArray<Bitboard, 64, 64> compute_connecting_squares() {
    MDArray<Bitboard, 64, 64> to_return;
    for (Square first_square = Square::A1; first_square != Square::NONE; first_square++) {
        for (Square second_square = Square::A1; second_square != Square::NONE; second_square++) {
            Bitboard b(second_square);
            int rank_diff = rank(first_square) - rank(second_square);
            int file_diff = file(first_square) - file(second_square);

            if (first_square == second_square) {
                b = Bitboard(second_square);
            } else if (rank(first_square) == rank(second_square)) {
                for (int8_t i = file(first_square) - constexpr_sign(file_diff); i != ((int8_t) file(second_square)); i -= constexpr_sign(file_diff)) {
                    // use int8_t to satisfy the compiler re loop iteration count
                    b |= get_position(rank(first_square), i);
                }
            } else if (file(first_square) == file(second_square)) {
                for (int8_t i = rank(first_square) - constexpr_sign(rank_diff); i != ((int8_t) rank(second_square)); i -= constexpr_sign(rank_diff)) {
                    b |= get_position(i, file(first_square));
                }
            } else {
                int min_square = constexpr_min(sq_to_int(first_square), sq_to_int(second_square));
                int max_square = constexpr_max(sq_to_int(first_square), sq_to_int(second_square));

                if (constexpr_abs(rank_diff) == constexpr_abs(file_diff)) {
                    if (constexpr_sign(rank_diff) == constexpr_sign(file_diff)) {
                        // from bottom left to top right
                        for (int i = 0; i < 64; i++) {
                            int abs_diff = constexpr_abs(sq_to_int(first_square) - i);
                            if ((abs_diff % 9) == 0 && i > min_square && i < max_square) {
                                b |= bit(i);
                            }
                        }
                    } else {
                        // from top left to bottom right
                        for (int i = 0; i < 64; i++) {
                            int abs_diff = constexpr_abs(sq_to_int(first_square) - i);
                            if ((abs_diff % 7) == 0 && i > min_square && i < max_square) {
                                b |= bit(i);
                            }
                        }
                    }
                }
            }
            to_return[sq_to_int(first_square)][sq_to_int(second_square)] = b;
        }
    }
    return to_return;
}

constexpr MDArray<Bitboard, 64, 64> MagicNumbers::ConnectingSquares = compute_connecting_squares();

consteval MDArray<Bitboard, 64, 64> compute_aligned_squares() {
    MDArray<Bitboard, 64, 64> to_return;
    for (Square first_square = Square::A1; first_square != Square::NONE; first_square++) {
        for (Square second_square = Square::A1; second_square != Square::NONE; second_square++) {
            Bitboard b = 0;
            int rank_diff = rank(first_square) - rank(second_square);
            int file_diff = file(first_square) - file(second_square);

            if (first_square == second_square) {
                b = 0;
            } else if (rank_diff == 0) {
                for (int i = 0; i < 8; i++) {
                    b |= get_position(rank(first_square), i);
                }
            } else if (file_diff == 0) {
                for (int i = 0; i < 8; i++) {
                    b |= get_position(i, file(first_square));
                }
            } else if (constexpr_abs(rank_diff) == constexpr_abs(file_diff)) {
                for (int i = 0; i < 64; i++) {
                    int abs_diff = constexpr_abs(sq_to_int(first_square) - i);
                    if ((abs_diff % ((constexpr_sign(rank_diff) == constexpr_sign(file_diff)) ? 9 : 7)) == 0) {
                        b |= bit(i);
                    }
                }
            }
            to_return[sq_to_int(first_square)][sq_to_int(second_square)] = b;
        }
    }
    return to_return;
}

constexpr MDArray<Bitboard, 64, 64> MagicNumbers::AlignedSquares = compute_aligned_squares();

consteval std::array<Bitboard, 64> generate_king_moves() {
    std::array<Bitboard, 64> to_return;
    for (int rk = 0; rk < 8; rk++) {
        for (int fl = 0; fl < 8; fl++) {
            Bitboard b = 0;
            for (int r = -1; r <= 1; r++) {
                for (int f = -1; f <= 1; f++) {
                    int r_rk = rk + r;
                    int f_fl = fl + f;
                    if (!((r == 0 && f == 0) || r_rk < 0 || f_fl < 0 || r_rk >= 8 || f_fl >= 8)) {
                        b |= get_position(r_rk, f_fl);
                    }
                }
            }
            to_return[sq_to_int(get_position(rk, fl))] = b;
        }
    }
    return to_return;
}

constexpr std::array<Bitboard, 64> MagicNumbers::KingMoves = generate_king_moves();



constexpr std::array<Bitboard, 64> MagicNumbers::KnightMoves = {
    0x20400UL,
    0x50800UL,
    0xa1100UL,
    0x142200UL,
    0x284400UL,
    0x508800UL,
    0xa01000UL,
    0x402000UL,
    0x2040004UL,
    0x5080008UL,
    0xa110011UL,
    0x14220022UL,
    0x28440044UL,
    0x50880088UL,
    0xa0100010UL,
    0x40200020UL,
    0x204000402UL,
    0x508000805UL,
    0xa1100110aUL,
    0x1422002214UL,
    0x2844004428UL,
    0x5088008850UL,
    0xa0100010a0UL,
    0x4020002040UL,
    0x20400040200UL,
    0x50800080500UL,
    0xa1100110a00UL,
    0x142200221400UL,
    0x284400442800UL,
    0x508800885000UL,
    0xa0100010a000UL,
    0x402000204000UL,
    0x2040004020000UL,
    0x5080008050000UL,
    0xa1100110a0000UL,
    0x14220022140000UL,
    0x28440044280000UL,
    0x50880088500000UL,
    0xa0100010a00000UL,
    0x40200020400000UL,
    0x204000402000000UL,
    0x508000805000000UL,
    0xa1100110a000000UL,
    0x1422002214000000UL,
    0x2844004428000000UL,
    0x5088008850000000UL,
    0xa0100010a0000000UL,
    0x4020002040000000UL,
    0x400040200000000UL,
    0x800080500000000UL,
    0x1100110a00000000UL,
    0x2200221400000000UL,
    0x4400442800000000UL,
    0x8800885000000000UL,
    0x100010a000000000UL,
    0x2000204000000000UL,
    0x4020000000000UL,
    0x8050000000000UL,
    0x110a0000000000UL,
    0x22140000000000UL,
    0x44280000000000UL,
    0x88500000000000UL,
    0x10a00000000000UL,
    0x20400000000000UL,
};

constexpr MDArray<Bitboard, 2, 64> MagicNumbers::PawnAttacks = {{
    {0x200,
    0x500,
    0xa00,
    0x1400,
    0x2800,
    0x5000,
    0xa000,
    0x4000,
    0x20000,
    0x50000,
    0xa0000,
    0x140000,
    0x280000,
    0x500000,
    0xa00000,
    0x400000,
    0x2000000,
    0x5000000,
    0xa000000,
    0x14000000,
    0x28000000,
    0x50000000,
    0xa0000000,
    0x40000000,
    0x200000000,
    0x500000000,
    0xa00000000,
    0x1400000000,
    0x2800000000,
    0x5000000000,
    0xa000000000,
    0x4000000000,
    0x20000000000,
    0x50000000000,
    0xa0000000000,
    0x140000000000,
    0x280000000000,
    0x500000000000,
    0xa00000000000,
    0x400000000000,
    0x2000000000000,
    0x5000000000000,
    0xa000000000000,
    0x14000000000000,
    0x28000000000000,
    0x50000000000000,
    0xa0000000000000,
    0x40000000000000,
    0x200000000000000,
    0x500000000000000,
    0xa00000000000000,
    0x1400000000000000,
    0x2800000000000000,
    0x5000000000000000,
    0xa000000000000000,
    0x4000000000000000,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0},
    {0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x2,
    0x5,
    0xa,
    0x14,
    0x28,
    0x50,
    0xa0,
    0x40,
    0x200,
    0x500,
    0xa00,
    0x1400,
    0x2800,
    0x5000,
    0xa000,
    0x4000,
    0x20000,
    0x50000,
    0xa0000,
    0x140000,
    0x280000,
    0x500000,
    0xa00000,
    0x400000,
    0x2000000,
    0x5000000,
    0xa000000,
    0x14000000,
    0x28000000,
    0x50000000,
    0xa0000000,
    0x40000000,
    0x200000000,
    0x500000000,
    0xa00000000,
    0x1400000000,
    0x2800000000,
    0x5000000000,
    0xa000000000,
    0x4000000000,
    0x20000000000,
    0x50000000000,
    0xa0000000000,
    0x140000000000,
    0x280000000000,
    0x500000000000,
    0xa00000000000,
    0x400000000000,
    0x2000000000000,
    0x5000000000000,
    0xa000000000000,
    0x14000000000000,
    0x28000000000000,
    0x50000000000000,
    0xa0000000000000,
    0x40000000000000}
}};