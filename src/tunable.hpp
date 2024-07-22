#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include "uci_options.hpp"

#ifdef IS_TUNE
#define TUNABLE_INT TunableInt
#define TUNABLE_FLOAT TunableFloat
#define TUNABLE_INT_CALLBACK TunableInt
#define TUNABLE_FLOAT_CALLBACK TunableFloat
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
    std::cout << this->name << ", " << "int" << ", " << this->value << ", " << this->min << ", " << this->max << ", " << this->step << ", " << this->learning_rate << std::endl;
    uci_options().insert(std::make_pair(this->name, UCIOption(this->min, this->max, std::to_string(this->value), UCIOptionTypes::TUNE_SPIN, [this, custom_callback](UCIOption& opt) { this->value = opt; custom_callback(); })));
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
    std::cout << this->name << ", " << "float" << ", " << this->value << ", " << this->min << ", " << this->max << ", " << this->step << ", " << this->learning_rate << std::endl;
    uci_options().insert(std::make_pair(this->name, UCIOption(this->min, this->max, std::to_string(this->value), UCIOptionTypes::TUNE_STRING, [this, custom_callback](UCIOption& opt) { this->value = std::stod(opt); custom_callback(); })));
}
#else
using TunableInt = int;
using TunableFloat = double;
#define TUNABLE_INT(name, val, min, max) val
#define TUNABLE_FLOAT(name, val, min, max) val
#define TUNABLE_INT_CALLBACK(name, val, min, max, learning_rate, callback) val
#define TUNABLE_FLOAT_CALLBACK(name, val, min, max, learning_rate, callback) val
#endif