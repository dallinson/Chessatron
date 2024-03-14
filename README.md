# Chessatron


## Credits

The pinned piece detection ([chessboard.cpp](src/chessboard.cpp#L475)) and move legality check ([move_generator.cpp](src/move_generator.cpp#98)) are taken from Stockfish

The FENs used for the bench in [search_handler.cpp](src/search_handler.cpp#L88) are taken from Alexandria

The static exchange evaluation in [search.cpp](src/search.cpp#L81) is taken from Ethereal

The evaluation terms were tuned using [GediminasMasaitis' texel-tuner](https://github.com/GediminasMasaitis/texel-tuner) using the lichess-big3-resolved dataset

The values used for move ordering in [move_ordering.cpp](src/move_ordering.cpp) are taken from Alexandria

The PVS implementation in [search.cpp](src/search.cpp#L273) is taken from Weiss

The [SEE tests](tests/see_tests.cpp) are from Starzix

Testing was performed using OpenBench

And most importantly, the development of this engine would not be possible without the brilliant people in the Stockfish Discord