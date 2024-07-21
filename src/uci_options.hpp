#pragma once

#include <string>
#include <functional>
#include <unordered_map>

class UCIOption;

inline auto& uci_options() {
    static std::unordered_map<std::string, UCIOption> options;
    return options;
}

enum class UCIOptionTypes {
    CHECK,
    SPIN,
    TUNE_SPIN,
    COMBO,
    BUTTON,
    STRING,
    TUNE_STRING,
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
        UCIOption(int min, int max, std::string default_value, UCIOptionTypes option_type, std::function<void(UCIOption&)> callback);
        UCIOption(int min, int max, std::string default_value, std::function<void(UCIOption&)> callback) : UCIOption(min, max, default_value, UCIOptionTypes::SPIN, callback) {};
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
