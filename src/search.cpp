#include "search.hpp"

#include <iostream>
#include <vector>

#include "move_generator.hpp"

template <bool print_debug>
uint64_t perft(ChessBoard& c, MoveHistory m, int depth, std::vector<std::unordered_map<ChessBoard, uint64_t>>& cache_vec) {

    if (cache_vec[depth - 1].contains(c)) {
        return cache_vec[depth - 1].at(c);
    }

    MoveList moves;
    uint64_t to_return = 0;

    moves = MoveGenerator::generate_legal_moves(c, c.get_side_to_move());

    if (depth == 1) {
        if (print_debug) [[unlikely]] {
            for (size_t i = 0; i < moves.len(); i++) {
                printf("%s: 1\n", moves[i].to_string().c_str());
            }
        }
        return moves.len();
    }

    for (size_t i = 0; i < moves.len(); i++) {
        uint64_t val;
        c.make_move(moves[i], m);
        val = perft<false>(c, m, depth - 1, cache_vec);
        if (print_debug) {
            std::cout << moves[i].to_string() << ": " << val << std::endl;
        }
        to_return += val;
        c.unmake_move(m);
    }

    cache_vec[depth - 1].try_emplace(c, to_return);
    return to_return;
}

uint64_t Perft::run_perft(ChessBoard& c, int depth, bool print_debug) {
    MoveHistory m;
    std::vector<std::unordered_map<ChessBoard, uint64_t>> cache_vec;
    cache_vec.resize(depth);
    int nodes = 0;
    if (print_debug) {
        nodes = perft<true>(c, m, depth, cache_vec);
    } else {
        nodes = perft<false>(c, m, depth, cache_vec);
    }
    if (print_debug) {
        std::cout << std::endl << "Nodes searched: " << nodes << std::endl;
    }
    return nodes;
}