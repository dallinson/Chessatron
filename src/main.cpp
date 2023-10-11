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
#include "search.hpp"
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

void process_position_command(const std::string& line, ChessBoard& c) {
    auto parsed_line = split_on_whitespace(line);
    int fen_idx;
    if (parsed_line[1] == "fen") {
        fen_idx = line.find("fen") + 3;
        while (std::isspace(line.at(fen_idx))) {
            fen_idx += 1;
        }
    } else if (parsed_line[1] == "startpos") {
        fen_idx = line.find("startpos");
    } else {
        return; // not a valid position
    }
    auto sub_line = line.substr(fen_idx);
    auto idx = c.set_from_fen(sub_line);
    if (idx.has_value()) {
        auto moves = sub_line.substr(idx.value());
        // moves is the substring starting at the end of the FEN string
        if (moves.find("moves") != std::string::npos) {
            // if there _are_ moves to add
            MoveHistory setup;
            auto split_moves = split_on_whitespace(moves.substr(moves.find(moves) + 5));
            for (auto& move : split_moves) {
                auto parsed_move = c.generate_move_from_string(move);
                if (parsed_move.has_value()) {
                    c.make_move(parsed_move.value(), setup);
                }
            }
        }
    }
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
                if (parsed_line[0] == std::string("go")) {
                    if (parsed_line.size() >= 3 && parsed_line[1] == std::string("perft")) {
                        Perft::run_perft(c, std::stoi(parsed_line[2]), true);
                    }
                } else if (parsed_line[0] == "position") {
                    process_position_command(line, c);
                }
            }
        }
    }
    return 0;
}