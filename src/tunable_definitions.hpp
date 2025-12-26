#pragma once

#include <array>

#include "tunable.hpp"
#include "utils.hpp"

constexpr auto default_see_pawn_value = 90;
constexpr auto default_see_knight_value = 268;
constexpr auto default_see_bishop_value = 301;
constexpr auto default_see_rook_value = 490;
constexpr auto default_see_queen_value = 937;

namespace Search {
    TUNABLE_SPECIFIER std::array<Score, 7> SEEScores = { 0, default_see_pawn_value, default_see_knight_value, default_see_bishop_value, default_see_rook_value, default_see_queen_value, 0 };
inline void update_see_values();

#ifdef IS_TUNE
TUNABLE_SPECIFIER TunableInt see_pawn_value = TUNABLE_INT_CALLBACK("see_pawn_value", default_see_pawn_value, 0, 200, 0.002, [](){ update_see_values(); });
TUNABLE_SPECIFIER TunableInt see_knight_value = TUNABLE_INT_CALLBACK("see_knight_value", default_see_knight_value, 100, 500, 0.002, [](){ update_see_values(); });
TUNABLE_SPECIFIER TunableInt see_bishop_value = TUNABLE_INT_CALLBACK("see_bishop_value", default_see_bishop_value, 100, 500, 0.002, [](){ update_see_values(); });
TUNABLE_SPECIFIER TunableInt see_rook_value = TUNABLE_INT_CALLBACK("see_rook_value", default_see_rook_value, 300, 700, 0.002, [](){ update_see_values(); });
TUNABLE_SPECIFIER TunableInt see_queen_value = TUNABLE_INT_CALLBACK("see_queen_value", default_see_queen_value, 700, 1100, 0.002, [](){ update_see_values(); });

inline void update_see_values() {
    SEEScores[1] = see_pawn_value;
    SEEScores[2] = see_knight_value;
    SEEScores[3] = see_bishop_value;
    SEEScores[4] = see_rook_value;
    SEEScores[5] = see_queen_value;
}
#endif
};

TUNABLE_SPECIFIER TunableInt ordering_noisy_history_divisor = TUNABLE_INT("ordering_noisy_history_divisor", 64, 4, 128);
TUNABLE_SPECIFIER TunableInt ordering_noisy_see_threshold_divisor = TUNABLE_INT("ordering_noisy_see_threshold_divisor", 4, 1, 16);