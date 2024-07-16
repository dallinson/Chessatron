#pragma once

#include <string>

#include "uci_options.hpp"

template<typename T>
class Tunable {
    private:
        std::string name;
        T min;
        T max;
        T value;
        T step;
        double learning_rate;
    public:
        Tunable(std::string_view name, T value, T min, T max, T step, double learning_rate = 0.002);
        operator T() const { return value; };
};

using TunableInt = Tunable<int>;
using TunableFloat = Tunable<double>;

template<>
inline TunableInt::Tunable(std::string_view name, int value, int min, int max, int step, double learning_rate) {
    this->name = name;
    this->min = min;
    this->max = max;
    this->value = value;
    this->step = step;
    this->learning_rate = learning_rate;
    #ifdef IS_TUNE 
    uci_options.insert(std::make_pair(this->name, UCIOption(this->min, this->max, std::to_string(this->value), UCIOptionTypes::TUNE_SPIN, [this](UCIOption& opt) { this->value = opt; })));
    #endif
}