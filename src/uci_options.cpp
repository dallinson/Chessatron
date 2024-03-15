#include "uci_options.hpp"

#include <iostream>
#include <string>

std::unordered_map<std::string, UCIOption> uci_options;

UCIOption::UCIOption(int min, int max, std::string default_value, std::function<void(UCIOption&)> callback) {
    this->option_type = UCIOptionTypes::SPIN;
    this->min = min;
    this->max = max;
    this->default_value = default_value;
    this->value = default_value;
    this->callback = callback;
}

UCIOption::operator int() const {
    if (this->option_type == UCIOptionTypes::SPIN) {
        return std::stoi(this->value);
    }
    return 0;
}

void UCIOption::set_value(std::string new_value) {
    this->value = new_value;
    this->callback(*this);
}

std::ostream& operator<<(std::ostream& out, const UCIOption& opt) {
    out << std::string(" type ");
    out << std::string(opt.get_type() == UCIOptionTypes::CHECK    ? "check"
                       : opt.get_type() == UCIOptionTypes::COMBO  ? "combo"
                       : opt.get_type() == UCIOptionTypes::BUTTON ? "button"
                       : opt.get_type() == UCIOptionTypes::STRING ? "string"
                                                                  : "");
    if (opt.get_type() == UCIOptionTypes::SPIN) {
        out << std::string("spin default ") << opt.get_default_value() << std::string(" min ") << std::to_string(opt.get_min())
            << std::string(" max ") << std::to_string(opt.get_max());
    }

    return out;
}