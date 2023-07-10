#include <cstdio>

#ifdef IS_TESTING
#include <gtest/gtest.h>
#endif

#include "chessboard.hpp"

#include "utils.hpp"
#include "magic_numbers.hpp"
#include "pieces.hpp"

#include "move_generator.hpp"

int main(int argc, char** argv) {
    #ifdef IS_TESTING
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    #endif

    ChessBoard c;
    //c.set_from_fen("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 ");
    c.set_from_fen("startpos");

    //c.print_board();

    print_bitboard(MoveGenerator::generate_bishop_movemask(c, 30));
    return 0;
}