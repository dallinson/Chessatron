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

void process_position_command(const std::string& line, ChessBoard& c, MoveHistory& m) {
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
    m = MoveHistory();
    if (idx.has_value()) {
        auto moves = sub_line.substr(idx.value());
        // moves is the substring starting at the end of the FEN string
        if (moves.find("moves") != std::string::npos) {
            // if there _are_ moves to add
            auto split_moves = split_on_whitespace(moves.substr(moves.find(moves) + 5));
            for (auto& move : split_moves) {
                auto parsed_move = c.generate_move_from_string(move);
                if (parsed_move.has_value()) {
                    c.make_move(parsed_move.value(), m);
                }
            }
        }
    }
}

void process_go_command(const std::vector<std::string>& line, SearchHandler& s) {
    int wtime = 0, btime = 0, winc = 0, binc = 0, movetime = 0;
    int movestogo = 1;
    int depth = MagicNumbers::PositiveInfinity;
    for (size_t i = 0; i < line.size(); i++) {
        auto& this_elem = line[i];
        if (this_elem == "infinite") {
            s.search(0);
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
            }
        }
    }
    if (movetime != 0) {
        s.search(movetime, depth);
        return;
    }
    auto current_side = s.get_board().get_side_to_move();
    // auto halfmoves_so_far = (2 * s.get_board().get_fullmove_counter()) + static_cast<int>(current_side);
    auto remaining_time = (current_side == Side::WHITE) ? wtime : btime;
    auto increment = ((current_side == Side::WHITE) ? winc : binc) / movestogo;
    // next we determine how to use our allocated time using the formula
    // 59.3 + (72830 - 2330 k)/(2644 + k (10 + k)), where k is the number of halfmoves
    // so far.  This formula is taken from https://chess.stackexchange.com/questions/2506/what-is-the-average-length-of-a-game-of-chess.
    // float remaining_halfmoves =
    //    59.3 + (static_cast<float>(72830 - (2330 * halfmoves_so_far)) / static_cast<float>(2644 + (halfmoves_so_far * (10 + halfmoves_so_far))));

    // s.search((remaining_time / static_cast<int>(remaining_halfmoves)) + increment, depth);
    s.search(remaining_time / 20 + increment / 2, depth);
}

int main(int argc, char** argv) {
#ifdef IS_TESTING
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
    srand(time(NULL));
    SearchHandler s;

    if (argc > 1) {
        if (std::string(argv[1]) == "bench") {
            if (argc > 2) {
                s.run_bench(std::stoi(argv[2]));
            } else {
                s.run_bench();
            }
            s.shutdown();
            return 0;
        }
    }

    for (std::string line; std::getline(std::cin, line);) {
        if (line == "uci") {
            std::cout << "id name Chessatron\n";
            std::cout << "option name Hash type spin default 16 min 16 max 16\n";
            std::cout << "option name Threads type spin default 1 min 1 max 1\n";
            std::cout << "uciok" << std::endl;
        } else if (line == "isready") {
            std::cout << "readyok\n";
        } else if (line == "ucinewgame") {
            s.reset();
        } else if (line == "quit") {
            s.shutdown();
            break;
        } else if (line == "stop") {
            s.EndSearch();
        } else {
            auto parsed_line = split_on_whitespace(line);
            if (parsed_line.size() >= 1) {
                if (parsed_line[0] == std::string("go")) {
                    process_go_command(parsed_line, s);
                } else if (parsed_line[0] == "position") {
                    process_position_command(line, s.get_board(), s.get_history());
                }
            }
        }
    }
    return 0;
}