#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifdef IS_TESTING
#include <gtest/gtest.h>
#endif

#include "chessboard.hpp"

#include "magic_numbers.hpp"
#include "pieces.hpp"
#include "utils.hpp"

#include "move_generator.hpp"

std::vector<std::string> split_on_whitespace(const std::string& data) {
    std::vector<std::string> to_return;
    std::string s;
    for (auto& c : data) {
        if (!std::isspace(c)) {
            s.push_back(c);
        } else if (s.length() != 0) {
            to_return.push_back(s);
            s = std::string();
        }
    }
    if (s.length() != 0) {
        to_return.push_back(s);
    }
    return to_return;
}

int main(int argc, char** argv) {
#ifdef IS_TESTING
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
    (void) argc;
    (void) argv;

    ChessBoard c;
    MoveHistory m;
    for (std::string line; std::getline(std::cin, line);) {
        if (line == "uci") {
            std::cout << "uciok\n";
        } else if (line == "isready") {
            std::cout << "readyok\n";
        } else if (line == "ucinewgame") {
            c = ChessBoard();
            m = MoveHistory();
        } else {
            auto parsed_line = split_on_whitespace(line);
            if (parsed_line.size() >= 1) {
            }
        }
    }
    return 0;
}