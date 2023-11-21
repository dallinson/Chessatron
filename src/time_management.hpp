#pragma once

#include <limits>
#include <variant>

#include <cstdint>

// Used for movestogo-based time control
struct FixedTimeTC;
// Used for wtime/btime/winc/binc-based time control
struct VariableTimeTC;
// Used for depth-based time control
struct DepthTC;
// Used for infinite time control
struct InfiniteTC {};

using TimeControlInfo = std::variant<FixedTimeTC, VariableTimeTC, DepthTC, InfiniteTC>;

struct FixedTimeTC {
    uint32_t search_time;
};

struct VariableTimeTC {
    uint32_t search_time;
    uint32_t side_time;
    uint32_t side_increment; 
};

struct DepthTC {
    uint16_t depth;
};

namespace TimeManagement {
    /**
     * @brief Determines if this std::variant is an instance of InfiniteTC
     * 
     * @param tc 
     * @return true 
     * @return false 
     */
    inline bool is_infinite_tc(const TimeControlInfo& tc) {
        return std::visit([](const auto& tc) {
            return std::is_same_v<std::decay_t<decltype(tc)>, InfiniteTC>;
        }, tc);
    }

    /**
     * @brief Determines if this std::variant is an instance of a time-based TC (i.e. not infinite or depth-based)
     * 
     * @param tc 
     * @return true 
     * @return false 
     */
    inline bool is_time_based_tc(const TimeControlInfo& tc) {
        return std::visit([](const auto& tc) {
            return std::is_same_v<std::decay_t<decltype(tc)>, FixedTimeTC> || std::is_same_v<std::decay_t<decltype(tc)>, VariableTimeTC>;
        }, tc);
    }

    /**
     * @brief Gets the depth of the search
     * 
     * @param tc 
     * @return uint16_t The search depth if this std::variant is a DepthTC object, otherwise the maximum uint16_t value
     */
    inline uint16_t get_search_depth(const TimeControlInfo& tc) {
        return std::visit([](const auto& tc) {
            if constexpr (std::is_same_v<std::decay_t<decltype(tc)>, DepthTC>) {
                return tc.depth;
            } else {
                return std::numeric_limits<uint16_t>::max();
            }
        }, tc);
    }

    /**
     * @brief Gets the maximum time of the search
     * 
     * @param tc 
     * @return uint32_t The search time if this is a time-based TC, otherwise the maximum uint32_t value
     */
    inline uint32_t get_search_time(const TimeControlInfo& tc) {
        return std::visit([](const auto& tc) {
            if constexpr (std::is_same_v<std::decay_t<decltype(tc)>, FixedTimeTC>) {
                return tc.search_time;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(tc)>, VariableTimeTC>) {
                return tc.search_time;
            } else {
                return std::numeric_limits<uint32_t>::max();
            }
        }, tc);
    }

    /**
     * @brief Calculates the hard limit of the search from the time of the side to move and the increment
     * 
     * @param side_time 
     * @param side_increment 
     * @return uint32_t 
     */
    inline uint32_t calculate_hard_limit(const uint32_t side_time, const uint32_t side_increment, const ChessBoard& board) {
        const auto midgame_time = side_time / 20 + side_increment / 2;

        auto halfmoves_so_far = (2 * board.get_fullmove_counter()) + static_cast<int>(board.get_side_to_move());
        const float remaining_halfmoves = 59.3 + (static_cast<float>(72830 - (2330 * halfmoves_so_far)) / static_cast<float>(2644 + (halfmoves_so_far * (10 + halfmoves_so_far))));
        const auto endgame_time = (side_time / static_cast<int>(remaining_halfmoves)) + side_increment;

        const auto midgame_phase = std::min(Evaluation::calculate_game_phase(board), (uint8_t) 24);
        const auto endgame_phase = 24 - midgame_phase;

        return ((midgame_phase * midgame_time) + (endgame_phase * endgame_time)) / 24;
    }

    /**
     * @brief Calculates the soft limit of the search from the search time
     * 
     * @param tc 
     * @return uint32_t 
     */
    inline uint32_t calculate_soft_limit(const TimeControlInfo& tc) {
        return (get_search_time(tc) / 10) * 3;
    }

};