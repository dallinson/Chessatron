#pragma once

#include <array>
#include <cstdint>
#include <limits>

#include "utils.hpp"

#define BISHOP_MOVES 512
#define ROOK_MOVES 4096

namespace MagicNumbers {
    extern const Bitboard RookMagics[64];
    extern const Bitboard BishopMagics[64];
    extern const Bitboard RookMasks[64];
    extern const Bitboard BishopMasks[64];

    extern const int RookBits[64];
    extern const int BishopBits[64];

    extern const std::array<Bitboard, 32768> BishopAttacks;
    extern const std::array<Bitboard, 262144> RookAttacks;

    extern const std::array<Bitboard, 64 * 64> ConnectingSquares;
    extern const std::array<Bitboard, 64 * 64> AlignedSquares;

    extern const std::array<Bitboard, 64> KingMoves;
    extern const Bitboard KnightMoves[64];
    extern const Bitboard PawnAttacks[128];

    const int32_t PositiveInfinity = 1073741823;
    // This is (2^30)-1; int32_t::max is (2^31)-1 so this affords some leeway for maths to be performed
    // If PositiveInfinity was (2^31)-1 then segfaults could occur as overflow means mates were missed
    // and checking for checks happens without a king
    const int32_t NegativeInfinity = -PositiveInfinity;
    // int32_t so a known constant size
} // namespace MagicNumbers