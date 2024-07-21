#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef IS_TESTING
#include <gtest/gtest.h>
#endif

#include "chessboard.hpp"
#include "common.hpp"
#include "magic_numbers.hpp"
#include "pieces.hpp"
#include "search.hpp"
#include "uci_options.hpp"
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

void process_position_command(const std::string& line, SearchHandler& s) {
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
    Position c;
    auto idx = c.set_from_fen(sub_line);
    if (idx.has_value()) {
        auto moves = sub_line.substr(idx.value());
        // moves is the substring starting at the end of the FEN string
        if (moves.find("moves") != std::string::npos) {
            // if there _are_ moves to add
            auto split_moves = split_on_whitespace(moves.substr(moves.find(moves) + 5));
            auto history = BoardHistory(c);
            for (auto& move : split_moves) {
                auto parsed_move = c.generate_move_from_string(move);
                if (parsed_move.has_value()) {
                    c = c.make_move(parsed_move.value(), history);
                }
            }
            s.set_history(history);
        } else {
            s.set_pos(c);
        }
    }
}

void process_go_command(const std::vector<std::string>& line, SearchHandler& s) {
    uint32_t wtime = 0, btime = 0, winc = 0, binc = 0, movetime = 0;
    uint32_t movestogo = 1;
    uint16_t depth = std::numeric_limits<uint16_t>::max();
    if (line.size() == 0) {
        s.search(InfiniteTC{});
        return;
    }
    for (size_t i = 0; i < line.size(); i++) {
        auto& this_elem = line[i];
        if (this_elem == "infinite") {
            s.search(InfiniteTC{});
            return;
        } else if (i != (line.size() - 1)) {
            if (this_elem == "wtime") {
                wtime = std::stoi(line[i + 1]);
            } else if (this_elem == "btime") {
                btime = std::stoi(line[i + 1]);
            } else if (this_elem == "winc") {
                winc = std::stoi(line[i + 1]);
            } else if (this_elem == "binc") {
                binc = std::stoi(line[i + 1]);
            } else if (this_elem == "movetime") {
                movetime = std::stoi(line[i + 1]);
            } else if (this_elem == "perft") {
                s.run_perft(std::stoi(line[i + 1]));
                return;
            } else if (this_elem == "movestogo") {
                movestogo = std::stoi(line[i + 1]);
            } else if (this_elem == "depth") {
                depth = std::stoi(line[i + 1]);
                s.search(DepthTC{depth});
                return;
            }
        }
    }
    if (movetime != 0) {
        s.search(FixedTimeTC{movetime});
        return;
    }
    const auto current_side = s.get_pos().stm();
    // auto halfmoves_so_far = (2 * s.get_pos().get_fullmove_counter()) + static_cast<int>(current_side);
    const auto remaining_time = (current_side == Side::WHITE) ? wtime : btime;
    const auto increment = ((current_side == Side::WHITE) ? winc : binc) / movestogo;
    // next we determine how to use our allocated time using the formula
    // 59.3 + (72830 - 2330 k)/(2644 + k (10 + k)), where k is the number of halfmoves
    // so far.  This formula is taken from https://chess.stackexchange.com/questions/2506/what-is-the-average-length-of-a-game-of-chess.
    // float remaining_halfmoves =
    //    59.3 + (static_cast<float>(72830 - (2330 * halfmoves_so_far)) / static_cast<float>(2644 + (halfmoves_so_far * (10 + halfmoves_so_far))));

    // s.search((remaining_time / static_cast<int>(remaining_halfmoves)) + increment, depth);
    s.search(VariableTimeTC{TimeManagement::calculate_hard_limit(remaining_time, increment), remaining_time, increment});
}

int main(int argc, char** argv) {
#ifdef IS_TESTING
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
    srand(time(NULL));
    SearchHandler s;

    uci_options().insert(std::make_pair("Hash", UCIOption(1, 2048, "16", [](UCIOption& opt) { tt.resize(size_t(opt)); })));
    uci_options().insert(std::make_pair("Threads", UCIOption(1, 1, "1", [](UCIOption& opt) { (void) opt; })));

    if (argc > 1) {
        if (std::string(argv[1]) == "bench") {
            if (argc > 2) {
                s.run_bench(std::stoi(argv[2]));
            } else {
                s.run_bench();
            }
            return 0;
        }
    }

    for (std::string line; std::getline(std::cin, line);) {
        if (line == "uci") {
            std::cout << "id name Chessatron\n";
            for (const auto& element : uci_options()) {
                std::cout << "option name " << element.first << element.second << std::endl;
            }
            std::cout << "uciok" << std::endl;
        } else if (line == "isready") {
            std::cout << "readyok\n";
        } else if (line == "ucinewgame") {
            s.reset();
        } else if (line == "quit") {
            break;
        } else if (line == "stop") {
            s.EndSearch();
        } else if (line == "d") {
            s.get_pos().print_board();
        } else if (line == "bench") {
            s.run_bench();
        } else {
            auto parsed_line = split_on_whitespace(line);
            if (parsed_line.size() >= 1) {
                if (parsed_line[0] == std::string("go")) {
                    process_go_command(parsed_line, s);
                } else if (parsed_line[0] == "position") {
                    process_position_command(line, s);
                } else if (parsed_line[0] == "setoption") {
                    std::istringstream iss(line);
                    std::string token, value, option_name;
                    iss >> token;
                    iss >> token;
                    while (iss >> token && token != "value") {
                        option_name += (option_name.empty() ? "" : " ");
                        option_name += token;
                    }
                    while (iss >> token) {
                        value += (value.empty() ? "" : " ");
                        value += token;
                    }
                    if (uci_options().contains(option_name)) {
                        uci_options()[option_name].set_value(value);
                    }
                }
            }
        }
    }
    return 0;
}