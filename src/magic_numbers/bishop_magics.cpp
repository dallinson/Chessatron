#include "../magic_numbers.hpp"

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

constexpr int MagicNumbers::BishopBits[64] = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                                              5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

constexpr Bitboard generate_bishop_attacks(Square square, Bitboard mask) {
    int rnk = rank(square);
    int fle = file(square);
    Bitboard to_return = 0;
    int r, f;
    for (r = rnk + 1, f = fle + 1; r <= 7 && f <= 7; r++, f++) {
        Bitboard b(get_position(r, f));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }
    for (r = rnk + 1, f = fle - 1; r <= 7 && f >= 0; r++, f--) {
        Bitboard b(get_position(r, f));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }
    for (r = rnk - 1, f = fle + 1; r >= 0 && f <= 7; r--, f++) {
        Bitboard b(get_position(r, f));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }
    for (r = rnk - 1, f = fle - 1; r >= 0 && f >= 0; r--, f--) {
        Bitboard b(get_position(r, f));
        to_return |= b;
        if (b & mask) {
            break;
        }
    }

    return to_return;
}

consteval MDArray<Bitboard, 64, 512> generate_bishop_attack_bitboards() {
    MDArray<Bitboard, 64, 512> to_return = {{{0}}};
    for (int square = 0; square < 64; square++) {
        Bitboard attack_mask = MagicNumbers::BishopMasks[square];
        std::array<Bitboard, 512> blockers = {0};
        std::array<Bitboard, 512> attacks = {0};
        for (int i = 0; i < 512; i++) {
            blockers[i] = 0;
            attacks[i] = 0;
        }
        Bitboard current_blockers = 0;
        for (int i = 0; i < (1 << attack_mask.popcnt()); i++) {
            blockers[i] = current_blockers;
            current_blockers = (current_blockers - attack_mask) & attack_mask;
            // bit twiddling trick
            attacks[i] = generate_bishop_attacks(static_cast<Square>(square), blockers[i]);
        }
        for (int i = 0; i < (1 << attack_mask.popcnt()); i++) {
            to_return[square][(blockers[i] * MagicNumbers::BishopMagics[square]) >> (64 - MagicNumbers::BishopBits[square])] = attacks[i];
        }
    }
    return to_return;
}

constexpr MDArray<Bitboard, 64, 512> MagicNumbers::BishopAttacks = generate_bishop_attack_bitboards();