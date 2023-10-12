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

    extern const std::array<std::array<Bitboard, 64>, 64> ConnectingSquares;
    extern const std::array<std::array<Bitboard, 64>, 64> AlignedSquares;

    extern const std::array<Bitboard, 64> KingMoves;
    extern const Bitboard KnightMoves[64];
    extern const Bitboard PawnAttacks[128];

    const int32_t NegativeInfinity = -std::numeric_limits<int32_t>::max();
    // int32_t so a known constant size and max as in a 32-bit int abs(highest value) is 1 lower than abs(lowest_value)
} // namespace MagicNumbers