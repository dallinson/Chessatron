#include "uci_options.hpp"

#include <iostream>
#include <string>

UCIOption::UCIOption(int min, int max, std::string default_value, UCIOptionTypes option_type, std::function<void(UCIOption&)> callback) {
    this->option_type = option_type;
    this->min = min;
    this->max = max;
    this->_default = default_value;
    this->value = default_value;
    this->callback = callback;
}

UCIOption::operator int() const {
    if (this->option_type == UCIOptionTypes::SPIN || this->option_type == UCIOptionTypes::TUNE_SPIN) {
        return std::stoi(this->value);
    }
    return 0;
}

UCIOption::operator std::string() const {
    if (this->option_type == UCIOptionTypes::STRING || this->option_type == UCIOptionTypes::TUNE_STRING) {
        return this->value;
    }
    return "";
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
                                                                  : "");
    if (opt.get_type() == UCIOptionTypes::SPIN || opt.get_type() == UCIOptionTypes::TUNE_SPIN) {
        out << std::string("spin default ") << opt.default_value() << std::string(" min ") << std::to_string(opt.get_min())
            << std::string(" max ") << std::to_string(opt.get_max());
    } else if (opt.get_type() == UCIOptionTypes::STRING || opt.get_type() == UCIOptionTypes::TUNE_STRING) {
        out << "string default " << ((opt.default_value() == "") ? "<empty>" : opt.default_value());
    }

    return out;
}