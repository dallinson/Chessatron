#include "uci_options.hpp"

#include <iostream>
#include <string>

UCIOption::UCIOption(int min, int max, std::string default_value, UCIOptionTypes option_type, std::function<void(UCIOption&)> callback) {
    this->option_type = option_type;
    this->min = min;
    this->max = max;
    this->_default = default_value;
    this->_value = default_value;
    this->callback = callback;
}

UCIOption::operator int() const {
    if (this->option_type == UCIOptionTypes::SPIN || this->option_type == UCIOptionTypes::TUNE_SPIN) {
        return std::stoi(this->_value);
    }
    return 0;
}

UCIOption::operator std::string() const {
    if (this->option_type == UCIOptionTypes::STRING || this->option_type == UCIOptionTypes::TUNE_STRING) {
        return this->_value;
    }
    return "";
}

UCIOption::operator bool() const {
    if (this->option_type == UCIOptionTypes::CHECK) {
        return this->_value == "true";
    }
    return static_cast<bool>(false);
}

void UCIOption::set_value(std::string new_value) {
    this->_value = new_value;
    this->callback(*this);
}

std::ostream& operator<<(std::ostream& out, const UCIOption& opt) {
    out << fmt::format("{}", opt);

    return out;
}