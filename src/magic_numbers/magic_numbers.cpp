#include "../magic_numbers.hpp"

constexpr Bitboard MagicNumbers::RookMagics[64] = {
    0x6080008040062850ULL, 0x1300204002810010ULL, 0x100110440082000ULL,  0x100050008100020ULL,  0x480022800240080ULL,  0xb00080201001400ULL,
    0x2800a0004800100ULL,  0x980042100004080ULL,  0x402002200804100ULL,  0x1003100400080ULL,    0x401001020010040ULL,  0x409002209001001ULL,
    0x2001001008010004ULL, 0x802001004020008ULL,  0x801000100040200ULL,  0x101000870820100ULL,  0x80004000200040ULL,   0xc00404010002000ULL,
    0xa40110020010048ULL,  0x1000210010010008ULL, 0x881030008001004ULL,  0x404008004020080ULL,  0x20906c0042881001ULL, 0x4020021005084ULL,
    0x4480004140002000ULL, 0x1050034a40006000ULL, 0xa0c401100200305ULL,  0x101a100100208ULL,    0x20080100100501ULL,   0x10040801402010ULL,
    0x4821000100040200ULL, 0x20208200040041ULL,   0x40018021800140ULL,   0x10002000400048ULL,   0x20001000802080ULL,   0x10028010800800ULL,
    0x2608008008800400ULL, 0xc006024011008ULL,    0x28104001048ULL,      0x28600c844a000401ULL, 0x80002000404009ULL,   0x2000200050094000ULL,
    0x4043022000110042ULL, 0x2063020810010020ULL, 0x2080100110004ULL,    0x806000805620030ULL,  0x20810040081ULL,      0x4008040041860009ULL,
    0x201004020800100ULL,  0x40308300400300ULL,   0x320001841002100ULL,  0x151092200104200ULL,  0x4201002800253100ULL, 0x400041020400801ULL,
    0x752008c210010400ULL, 0x44109051040200ULL,   0x1711008002514063ULL, 0x40c1008020400011ULL, 0x60010010200841ULL,   0x550002008110105ULL,
    0x1000410020801ULL,    0x240100080204000bULL, 0x600050220088410cULL, 0x2c011400852442ULL,
};

constexpr Bitboard MagicNumbers::BishopMagics[64] = {
    0x1004040846040012ULL, 0x84410851070008ULL,   0x610008208400808ULL,  0x114504201221105ULL,  0x21104100004000ULL,   0x2482004012800ULL,
    0x1002011002100004ULL, 0x410402048a2800ULL,   0x80c310a08070408ULL,  0x4082208121828ULL,    0x100418484010ULL,     0x221004106200000aULL,
    0x9040420000046ULL,    0x8008210404000ULL,    0x110020201200810ULL,  0x76010108220300ULL,   0x2a00010049010c1ULL,  0x882000450040104ULL,
    0x10003200220821ULL,   0x8401401420002ULL,    0x19014820082100ULL,   0x19000a00410404ULL,   0x5031002054100502ULL, 0x40210053080810ULL,
    0x220082320c80120ULL,  0x410084210020084ULL,  0x130022042404040aULL, 0x84040020101010ULL,   0x81040002002100ULL,   0xa08410002100210ULL,
    0x4004006001180200ULL, 0x801004041004800ULL,  0x138059000442008ULL,  0x4922020200202840ULL, 0x4002004051040101ULL, 0x48a2202020180081ULL,
    0x10020202002008ULL,   0x10120020020084ULL,   0x8080054010108ULL,    0x2020020084400ULL,    0x202120220004200ULL,  0xc1901104291100eULL,
    0x689008044004040ULL,  0x400056204212800ULL,  0xc02080104020040ULL,  0xc0010060800101ULL,   0x130022a04001040ULL,  0x8400821a000040ULL,
    0x2020411460201014ULL, 0x4203048809180130ULL, 0x3002088058084940ULL, 0x500000722a080006ULL, 0x2406008504120ULL,    0x1088044830031310ULL,
    0x9200102020202ULL,    0x4808101952012002ULL, 0x5001040842021010ULL, 0x10c8090118020200ULL, 0x4008008222091001ULL, 0x800000000840404ULL,
    0x72040850108ULL,      0x401001041810490cULL, 0x10c040458060430ULL,  0x1c1042102102100ULL,
};

constexpr Bitboard MagicNumbers::RookMasks[64] = {
    0x101010101017eULL,    0x202020202027cULL,    0x404040404047aULL,    0x8080808080876ULL,    0x1010101010106eULL,   0x2020202020205eULL,
    0x4040404040403eULL,   0x8080808080807eULL,   0x1010101017e00ULL,    0x2020202027c00ULL,    0x4040404047a00ULL,    0x8080808087600ULL,
    0x10101010106e00ULL,   0x20202020205e00ULL,   0x40404040403e00ULL,   0x80808080807e00ULL,   0x10101017e0100ULL,    0x20202027c0200ULL,
    0x40404047a0400ULL,    0x8080808760800ULL,    0x101010106e1000ULL,   0x202020205e2000ULL,   0x404040403e4000ULL,   0x808080807e8000ULL,
    0x101017e010100ULL,    0x202027c020200ULL,    0x404047a040400ULL,    0x8080876080800ULL,    0x1010106e101000ULL,   0x2020205e202000ULL,
    0x4040403e404000ULL,   0x8080807e808000ULL,   0x1017e01010100ULL,    0x2027c02020200ULL,    0x4047a04040400ULL,    0x8087608080800ULL,
    0x10106e10101000ULL,   0x20205e20202000ULL,   0x40403e40404000ULL,   0x80807e80808000ULL,   0x17e0101010100ULL,    0x27c0202020200ULL,
    0x47a0404040400ULL,    0x8760808080800ULL,    0x106e1010101000ULL,   0x205e2020202000ULL,   0x403e4040404000ULL,   0x807e8080808000ULL,
    0x7e010101010100ULL,   0x7c020202020200ULL,   0x7a040404040400ULL,   0x76080808080800ULL,   0x6e101010101000ULL,   0x5e202020202000ULL,
    0x3e404040404000ULL,   0x7e808080808000ULL,   0x7e01010101010100ULL, 0x7c02020202020200ULL, 0x7a04040404040400ULL, 0x7608080808080800ULL,
    0x6e10101010101000ULL, 0x5e20202020202000ULL, 0x3e40404040404000ULL, 0x7e80808080808000ULL,
};

constexpr Bitboard MagicNumbers::BishopMasks[64] = {
    0x40201008040200ULL, 0x402010080400ULL,   0x4020100a00ULL,     0x40221400ULL,       0x2442800ULL,        0x204085000ULL,      0x20408102000ULL,
    0x2040810204000ULL,  0x20100804020000ULL, 0x40201008040000ULL, 0x4020100a0000ULL,   0x4022140000ULL,     0x244280000ULL,      0x20408500000ULL,
    0x2040810200000ULL,  0x4081020400000ULL,  0x10080402000200ULL, 0x20100804000400ULL, 0x4020100a000a00ULL, 0x402214001400ULL,   0x24428002800ULL,
    0x2040850005000ULL,  0x4081020002000ULL,  0x8102040004000ULL,  0x8040200020400ULL,  0x10080400040800ULL, 0x20100a000a1000ULL, 0x40221400142200ULL,
    0x2442800284400ULL,  0x4085000500800ULL,  0x8102000201000ULL,  0x10204000402000ULL, 0x4020002040800ULL,  0x8040004081000ULL,  0x100a000a102000ULL,
    0x22140014224000ULL, 0x44280028440200ULL, 0x8500050080400ULL,  0x10200020100800ULL, 0x20400040201000ULL, 0x2000204081000ULL,  0x4000408102000ULL,
    0xa000a10204000ULL,  0x14001422400000ULL, 0x28002844020000ULL, 0x50005008040200ULL, 0x20002010080400ULL, 0x40004020100800ULL, 0x20408102000ULL,
    0x40810204000ULL,    0xa1020400000ULL,    0x142240000000ULL,   0x284402000000ULL,   0x500804020000ULL,   0x201008040200ULL,   0x402010080400ULL,
    0x2040810204000ULL,  0x4081020400000ULL,  0xa102040000000ULL,  0x14224000000000ULL, 0x28440200000000ULL, 0x50080402000000ULL, 0x20100804020000ULL,
    0x40201008040200ULL,
};

constexpr int MagicNumbers::RookBits[64] = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
                                            10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                                            10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

constexpr int MagicNumbers::BishopBits[64] = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                                              5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

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
    std::array<std::array<Bitboard, 64>, 64> to_return = {0};
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

constexpr Bitboard MagicNumbers::KingMoves[64] = {
    0x302ULL,
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
    0x40c0000000000000ULL
};

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