#pragma once

#include <array>

class Bitboard: private std::array<uint64_t, 2> {

    public:
    using std::array<uint64_t, 2>::operator[];

    uint64_t get_occupancy() const { return _Elems[0] | _Elems[1]; };

    Bitboard() { _Elems[0] = 0; _Elems[1] = 0; };
};