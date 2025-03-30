#pragma once

#include <string>
#include <fmt/format.h>
#include <functional>
#include <unordered_map>

class UCIOption;

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
        std::string _default;
        std::string _value;
        std::function<void(UCIOption&)> callback;
    public:
        UCIOption() : option_type(UCIOptionTypes::SPIN), min(0), max(0), _default("0"), _value("0"), callback([](UCIOption& opt) {(void) opt;}) {};
        UCIOption(int min, int max, std::string default_value, UCIOptionTypes option_type, std::function<void(UCIOption&)> callback);
        UCIOption(int min, int max, std::string default_value, std::function<void(UCIOption&)> callback) : UCIOption(min, max, default_value, UCIOptionTypes::SPIN, callback) {};
        operator int() const;
        operator std::string() const;
        operator bool() const;

        void set_value(std::string new_value);

        UCIOptionTypes get_type() const { return this->option_type; };
        std::string default_value() const { return this->_default; };
        int get_min() const { return this->min; };
        int get_max() const { return this->max; };
        std::string value() const { return this->_value; };
};

std::ostream& operator<<(std::ostream& out, const UCIOption& opt);

inline auto& uci_options() {
    static std::unordered_map<std::string, UCIOption> options;
    return options;
}

template <>
struct fmt::formatter<UCIOption> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const UCIOption& opt, fmt::format_context& ctx) const {
        std::string to_return = " type ";
        to_return += std::string(opt.get_type() == UCIOptionTypes::COMBO  ? "combo"
                                 : opt.get_type() == UCIOptionTypes::BUTTON ? "button"
                                                                            : "");
        if (opt.get_type() == UCIOptionTypes::SPIN || opt.get_type() == UCIOptionTypes::TUNE_SPIN) {
            to_return += fmt::format("spin default {} min {} max {}", opt.default_value(), opt.get_min(), opt.get_max());
        } else if (opt.get_type() == UCIOptionTypes::STRING || opt.get_type() == UCIOptionTypes::TUNE_STRING) {
            to_return += fmt::format("string default {}", (opt.default_value() == "") ? "<empty>" : opt.default_value());
        } else if (opt.get_type() == UCIOptionTypes::CHECK) {
            to_return += fmt::format("check default {}", opt.default_value());
        }
        return fmt::format_to(ctx.out(), "{}", to_return);
    }
};
