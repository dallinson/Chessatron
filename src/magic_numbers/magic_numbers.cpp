#include "../magic_numbers.hpp"

#define GET_SIGN(x) (((x) > 0) - ((x) < 0))
// clever way of doing a sign function, from https://stackoverflow.com/a/1903975
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
// pre-C++23 these functions are not constexpr so are reimplemented here

/**
 * @brief This generates the squares between a first square and a second square for each pair of squares, as used in the pin detection algorithm used
 * in Stockfish and reimplemented here.  If the two spaces have a line between them, the attacking/pinning piece is a slider and the attack can be
 * stopped by moving into any of these spaces or capturing the attacking piece.  If not, the attack can only be stopped by capturing the attacking
 * piece
 *
 * @return consteval
 */
consteval std::array<std::array<Bitboard, 64>, 64> compute_connecting_squares() {
    std::array<std::array<Bitboard, 64>, 64> to_return;
    for (int first_square = 0; first_square < 64; first_square++) {
        std::array<Bitboard, 64> inner_array;
        for (int second_square = 0; second_square < 64; second_square++) {
            Bitboard b = idx_to_bitboard(second_square);
            int rank_diff = GET_RANK(first_square) - GET_RANK(second_square);
            int file_diff = GET_FILE(first_square) - GET_FILE(second_square);

            if (first_square == second_square) {
                b = idx_to_bitboard(second_square);
            } else if (GET_RANK(first_square) == GET_RANK(second_square)) {
                for (int8_t i = GET_FILE(first_square) - GET_SIGN(file_diff); i != ((int8_t) GET_FILE(second_square)); i -= GET_SIGN(file_diff)) {
                    // use int8_t to satisfy the compiler re loop iteration count
                    b |= idx_to_bitboard(POSITION(GET_RANK(first_square), i));
                }
            } else if (GET_FILE(first_square) == GET_FILE(second_square)) {
                for (int8_t i = GET_RANK(first_square) - GET_SIGN(rank_diff); i != ((int8_t) GET_RANK(second_square)); i -= GET_SIGN(rank_diff)) {
                    b |= idx_to_bitboard(POSITION(i, GET_FILE(first_square)));
                }
            } else {
                int min_square = MIN(first_square, second_square);
                int max_square = MAX(first_square, second_square);

                if (ABS(rank_diff) == ABS(file_diff)) {
                    if (GET_SIGN(rank_diff) == GET_SIGN(file_diff)) {
                        // from bottom left to top right
                        for (int i = 0; i < 64; i++) {
                            int abs_diff = ABS(first_square - i);
                            if ((abs_diff % 9) == 0 && i > min_square && i < max_square) {
                                b |= BIT(i);
                            }
                        }
                    } else {
                        // from top left to bottom right
                        for (int i = 0; i < 64; i++) {
                            int abs_diff = ABS(first_square - i);
                            if ((abs_diff % 7) == 0 && i > min_square && i < max_square) {
                                b |= BIT(i);
                            }
                        }
                    }
                }
            }
            inner_array[second_square] = b;
        }
        to_return[first_square] = inner_array;
    }
    return to_return;
}

constexpr std::array<std::array<Bitboard, 64>, 64> MagicNumbers::ConnectingSquares = compute_connecting_squares();

consteval std::array<std::array<Bitboard, 64>, 64> compute_aligned_squares() {
    std::array<std::array<Bitboard, 64>, 64> to_return;
    for (int first_square = 0; first_square < 64; first_square++) {
        std::array<Bitboard, 64> inner_array = {0};
        for (int second_square = 0; second_square < 64; second_square++) {
            Bitboard b = 0;
            int rank_diff = GET_RANK(first_square) - GET_RANK(second_square);
            int file_diff = GET_FILE(first_square) - GET_FILE(second_square);

            if (first_square == second_square) {
                b = 0;
            } else if (rank_diff == 0) {
                for (int i = 0; i < 8; i++) {
                    b |= idx_to_bitboard(POSITION(GET_RANK(first_square), i));
                }
            } else if (file_diff == 0) {
                for (int i = 0; i < 8; i++) {
                    b |= idx_to_bitboard(POSITION(i, GET_FILE(first_square)));
                }
            } else if (ABS(rank_diff) == ABS(file_diff)) {
                for (int i = 0; i < 64; i++) {
                    int abs_diff = ABS(first_square - i);
                    if ((abs_diff % ((GET_SIGN(rank_diff) == GET_SIGN(file_diff)) ? 9 : 7)) == 0) {
                        b |= BIT(i);
                    }
                }
            }
            inner_array[second_square] = b;
        }
        to_return[first_square] = inner_array;
    }
    return to_return;
}

constexpr std::array<std::array<Bitboard, 64>, 64> MagicNumbers::AlignedSquares = compute_aligned_squares();

constexpr Bitboard MagicNumbers::KingMoves[64] = {0x302ULL,
                                                  0x705ULL,
                                                  0xe0aULL,
                                                  0x1c14ULL,
                                                  0x3828ULL,
                                                  0x7050ULL,
                                                  0xe0a0ULL,
                                                  0xc040ULL,
                                                  0x30200ULL,
                                                  0x70507ULL,
                                                  0xe0a0eULL,
                                                  0x1c141cULL,
                                                  0x382838ULL,
                                                  0x705070ULL,
                                                  0xe0a0e0ULL,
                                                  0xc040c0ULL,
                                                  0x3020300ULL,
                                                  0x7050700ULL,
                                                  0xe0a0e00ULL,
                                                  0x1c141c00ULL,
                                                  0x38283800ULL,
                                                  0x70507000ULL,
                                                  0xe0a0e000ULL,
                                                  0xc040c000ULL,
                                                  0x302030000ULL,
                                                  0x705070000ULL,
                                                  0xe0a0e0000ULL,
                                                  0x1c141c0000ULL,
                                                  0x3828380000ULL,
                                                  0x7050700000ULL,
                                                  0xe0a0e00000ULL,
                                                  0xc040c00000ULL,
                                                  0x30203000000ULL,
                                                  0x70507000000ULL,
                                                  0xe0a0e000000ULL,
                                                  0x1c141c000000ULL,
                                                  0x382838000000ULL,
                                                  0x705070000000ULL,
                                                  0xe0a0e0000000ULL,
                                                  0xc040c0000000ULL,
                                                  0x3020300000000ULL,
                                                  0x7050700000000ULL,
                                                  0xe0a0e00000000ULL,
                                                  0x1c141c00000000ULL,
                                                  0x38283800000000ULL,
                                                  0x70507000000000ULL,
                                                  0xe0a0e000000000ULL,
                                                  0xc040c000000000ULL,
                                                  0x302030000000000ULL,
                                                  0x705070000000000ULL,
                                                  0xe0a0e0000000000ULL,
                                                  0x1c141c0000000000ULL,
                                                  0x3828380000000000ULL,
                                                  0x7050700000000000ULL,
                                                  0xe0a0e00000000000ULL,
                                                  0xc040c00000000000ULL,
                                                  0x203000000000000ULL,
                                                  0x507000000000000ULL,
                                                  0xa0e000000000000ULL,
                                                  0x141c000000000000ULL,
                                                  0x2838000000000000ULL,
                                                  0x5070000000000000ULL,
                                                  0xa0e0000000000000ULL,
                                                  0x40c0000000000000ULL};

constexpr Bitboard MagicNumbers::KnightMoves[64] = {
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

constexpr Bitboard MagicNumbers::PawnAttacks[128] = {
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
    0x0,
    0x0,
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
    0x40000000000000,
};