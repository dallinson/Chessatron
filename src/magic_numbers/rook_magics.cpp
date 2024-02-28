#include <cstdint>

#include "../magic_numbers.hpp"
#include "../utils.hpp"

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

constexpr int MagicNumbers::RookBits[64] = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
                                            10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                                            10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

constexpr Bitboard generate_rook_attacks(int square, Bitboard mask) {
    Bitboard to_return = 0;
    int rank = get_rank(square);
    int file = get_file(square);
    for (int r = rank + 1; r <= 7; r++) {
        Bitboard b = idx_to_bb(get_position(r, file));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }
    for (int r = rank - 1; r >= 0; r--) {
        Bitboard b = idx_to_bb(get_position(r, file));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }

    for (int f = file + 1; f <= 7; f++) {
        Bitboard b = idx_to_bb(get_position(rank, f));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }
    for (int f = file - 1; f >= 0; f--) {
        Bitboard b = idx_to_bb(get_position(rank, f));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }

    return to_return;
}

consteval std::array<Bitboard, 64 * 4096> generate_rook_attack_bitboards() {
    std::array<Bitboard, 64 * 4096> to_return = {0};
    for (int square = 0; square < 64; square++) {
        Bitboard attack_mask = MagicNumbers::RookMasks[square];
        std::array<Bitboard, 4096> blockers = {0};
        std::array<Bitboard, 4096> attacks = {0};
        for (int i = 0; i < 4096; i++) {
            blockers[i] = 0;
            attacks[i] = 0;
        }
        Bitboard current_blockers = 0;
        for (int i = 0; i < (1 << std::popcount(attack_mask)); i++) {
            blockers[i] = current_blockers;
            current_blockers = (current_blockers - attack_mask) & attack_mask;
            // bit twiddling trick
            attacks[i] = generate_rook_attacks(square, blockers[i]);
        }
        for (int i = 0; i < (1 << std::popcount(attack_mask)); i++) {
            int j = (4096 * square) + ((blockers[i] * MagicNumbers::RookMagics[square]) >> (64 - MagicNumbers::RookBits[square]));
            to_return[j] = attacks[i];
        }
    }
    return to_return;
}

constexpr std::array<Bitboard, 262144> MagicNumbers::RookAttacks = generate_rook_attack_bitboards();