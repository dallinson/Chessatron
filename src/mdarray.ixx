module;

#include <array>

export module mdarray;

// The code in this file is taken from Stormphrax's multiarray

template <typename T, size_t N, size_t... Ns>
struct MDArrayImpl {
    using Type = std::array<typename MDArrayImpl<T, Ns...>::Type, N>;
};

template <typename T, size_t N>
struct MDArrayImpl<T, N> {
    using Type = std::array<T, N>;
};

export template <typename T, size_t... Ns>
using MDArray = MDArrayImpl<T, Ns...>::Type;