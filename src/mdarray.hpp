#pragma once
#include <array>

// The code in this file is taken from Stormphrax's multiarray

namespace mdarray_internal {
    template <typename T, size_t N, size_t... Ns>
    struct MDArrayImpl {
        using Type = std::array<typename MDArrayImpl<T, Ns...>::Type, N>;
    };

    template <typename T, size_t N>
    struct MDArrayImpl<T, N> {
        using Type = std::array<T, N>;
    };
};

template <typename T, size_t... Ns>
using MDArray = mdarray_internal::MDArrayImpl<T, Ns...>::Type;