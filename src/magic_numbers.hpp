#pragma once

#include <cstdint>
#include <array>

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

    extern const Bitboard BishopAttacks[32768];
    extern const Bitboard RookAttacks[262144];

    extern const std::array<std::array<Bitboard, 64>, 64> ConnectingSquares;
    extern const std::array<std::array<Bitboard, 64>, 64> AlignedSquares;

    extern const Bitboard KingMoves[64];
    extern const Bitboard KnightMoves[64];
    extern const Bitboard PawnAttacks[128];
}