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

    extern std::array<double, 256> LnValues;

    void generate_magics();

    constexpr int16_t PositiveInfinity = 16000;
    constexpr int16_t NegativeInfinity = -PositiveInfinity;
    // int32_t so a known constant size
} // namespace MagicNumbers