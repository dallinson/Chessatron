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

enum TCType {
    FIXED_TIME_TC,
    VARIABLE_TIME_TC,
    DEPTH_TC,
    INFINITE_TC,
};

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

struct TimeControlInfo {
    TCType time_control_type;
    union {
        FixedTimeTC fixed_time_tc;
        VariableTimeTC variable_time_tc;
        DepthTC depth_tc;
        InfiniteTC infinite_tc;
    };

    constexpr TimeControlInfo();
    constexpr TimeControlInfo(FixedTimeTC fixed_tc) : time_control_type(TCType::FIXED_TIME_TC), fixed_time_tc(fixed_tc) {};
    constexpr TimeControlInfo(VariableTimeTC variable_tc) : time_control_type(TCType::VARIABLE_TIME_TC), variable_time_tc(variable_tc) {};
    constexpr TimeControlInfo(DepthTC depth_tc) : time_control_type(TCType::DEPTH_TC), depth_tc(depth_tc) {};
    constexpr TimeControlInfo(InfiniteTC infinite_tc) : time_control_type(TCType::INFINITE_TC), infinite_tc(infinite_tc) {};
};

constexpr inline TimeControlInfo::TimeControlInfo() {
    *this = TimeControlInfo(InfiniteTC {});
}

namespace TimeManagement {
    /**
     * @brief Determines if this std::variant is an instance of InfiniteTC
     * 
     * @param tc 
     * @return true 
     * @return false 
     */
    inline bool is_infinite_tc(const TimeControlInfo& tc) {
        return tc.time_control_type == TCType::INFINITE_TC;
    }

    /**
     * @brief Determines if this std::variant is an instance of a time-based TC (i.e. not infinite or depth-based)
     * 
     * @param tc 
     * @return true 
     * @return false 
     */
    inline bool is_time_based_tc(const TimeControlInfo& tc) {
        return tc.time_control_type == TCType::FIXED_TIME_TC || tc.time_control_type == TCType::VARIABLE_TIME_TC;
    }

    /**
     * @brief Gets the depth of the search
     * 
     * @param tc 
     * @return uint16_t The search depth if this std::variant is a DepthTC object, otherwise the maximum uint16_t value
     */
    inline uint16_t get_search_depth(const TimeControlInfo& tc) {
        return (tc.time_control_type == TCType::DEPTH_TC) ? tc.depth_tc.depth : 250;
    }

    /**
     * @brief Gets the maximum time of the search
     * 
     * @param tc 
     * @return uint32_t The search time if this is a time-based TC, otherwise the maximum uint32_t value
     */
    inline uint32_t get_search_time(const TimeControlInfo& tc) {
        switch (tc.time_control_type) {
            case TCType::FIXED_TIME_TC:
                return tc.fixed_time_tc.search_time;
            case TCType::VARIABLE_TIME_TC:
                return tc.variable_time_tc.search_time;
            default:
                return std::numeric_limits<uint32_t>::max();
        }
    }

    /**
     * @brief Calculates the hard limit of the search from the time of the side to move and the increment
     * 
     * @param side_time 
     * @param side_increment 
     * @return uint32_t 
     */
    inline uint32_t calculate_hard_limit(const uint32_t side_time, const uint32_t side_increment) {
        return side_time / 20 + side_increment / 2;
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