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
        double step;
        double learning_rate;
    public:
        Tunable(std::string_view name, T value, T min, T max, double desired_learning_rate = 0.002, std::function<void(void)> custom_callback = [](){});
        operator T() const { return value; };
};

using TunableInt = Tunable<int>;
using TunableFloat = Tunable<double>;


template<>
inline TunableInt::Tunable(std::string_view name, int value, int min, int max, double desired_learning_rate, std::function<void(void)> custom_callback) {
    assert(max > min);
    this->name = name;
    this->min = min;
    this->max = max;
    this->value = value;
    this->step = ((double) (max - min)) / 20;
    this->learning_rate = desired_learning_rate;
    if (this->step < 0.5) {
        this->learning_rate *= 2 * this->step;
    }
    #ifdef IS_TUNE 
    uci_options().insert(std::make_pair(this->name, UCIOption(this->min, this->max, std::to_string(this->value), UCIOptionTypes::TUNE_SPIN, [this, custom_callback](UCIOption& opt) { this->value = opt; custom_callback(); })));
    #else 
    (void) custom_callback;
    #endif
}

template<>
inline TunableFloat::Tunable(std::string_view name, double value, double min, double max, double desired_learning_rate, std::function<void(void)> custom_callback) {
    assert(max > min);
    this->name = name;
    this->min = min;
    this->max = max;
    this->value = value;
    this->step = (max - min) / 20;
    this->learning_rate = desired_learning_rate;
    #ifdef IS_TUNE 
    uci_options().insert(std::make_pair(this->name, UCIOption(this->min, this->max, std::to_string(this->value), UCIOptionTypes::TUNE_STRING, [this, custom_callback](UCIOption& opt) { this->value = std::stod(opt); custom_callback(); })));
    #else 
    (void) custom_callback;
    #endif

}