#pragma once

#include <string>
#include <functional>
#include <unordered_map>

enum class UCIOptionTypes {
    CHECK,
    SPIN,
    COMBO,
    BUTTON,
    STRING,
};

class UCIOption {
    private:
        UCIOptionTypes option_type;
        int min;
        int max;
        std::string default_value;
        std::string value;
        std::function<void(UCIOption&)> callback;
    public:
        UCIOption() : option_type(UCIOptionTypes::SPIN), min(0), max(0), default_value("0"), value("0"), callback([](UCIOption& opt) {(void) opt;}) {};
        UCIOption(int min, int max, std::string default_value, std::function<void(UCIOption&)> callback);
        operator int() const;

        void set_value(std::string new_value);

        UCIOptionTypes get_type() const { return this->option_type; };
        std::string get_default_value() const { return this->default_value; };
        int get_min() const { return this->min; };
        int get_max() const { return this->max; };
};

std::ostream& operator<<(std::ostream& out, const UCIOption& opt);
inline auto& uci_options() {
    static std::unordered_map<std::string, UCIOption> options;
    return options;
}