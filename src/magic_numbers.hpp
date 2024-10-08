#pragma once

#include <array>
#include <cstdint>
#include <limits>

#include "bitboard.hpp"
#include "mdarray.hpp"
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

    extern const MDArray<Bitboard, 64, 512> BishopAttacks;
    extern const MDArray<Bitboard, 64, 4096> RookAttacks;

    extern const MDArray<Bitboard, 64, 64> ConnectingSquares;
    extern const MDArray<Bitboard, 64, 64> AlignedSquares;

    extern const std::array<Bitboard, 64> KingMoves;
    extern const std::array<Bitboard, 64> KnightMoves;
    extern const MDArray<Bitboard, 2, 64> PawnAttacks;

    void generate_magics();

    constexpr int16_t PositiveInfinity = 16000;
    constexpr int16_t NegativeInfinity = -PositiveInfinity;
    // int32_t so a known constant size
} // namespace MagicNumbers

constexpr static int MATE_FOUND = MagicNumbers::PositiveInfinity - MAX_PLY;