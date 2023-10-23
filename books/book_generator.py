import chess
import chess.pgn
import chess.polyglot
import os
import multiprocessing
from typing import Tuple, Dict, List
import argparse
import tqdm
import math

HERE_PATH = os.path.dirname(os.path.abspath(__file__))
WHITE_VICTORY = "1-0"
BLACK_VICTORY = "0-1"
VICTORY_WEIGHT = 2
DRAW = "1/2-1/2"
DRAW_WEIGHT = 1
count = 0

def move_to_polyglot(move: chess.Move) -> int:
    to_return = 0
    to_return |= move.to_square
    to_return |= (move.from_square) << 6

    promotion_piece = 0
    if move.promotion is not None:
        promotion_piece = move.promotion - 1
    to_return |= (promotion_piece << 12)

    return to_return


def read_file(args: Tuple[str, int]):
    file_name = args[0]
    move_depth = args[1]
    to_return: Dict[int, Dict[chess.Move, int]] = {}
    with open(os.path.join(HERE_PATH, file_name)) as opened_file:
        games_parsed = 0
        while game := chess.pgn.read_game(opened_file):
            games_parsed += 1
            board = game.board()
            result = game.headers["Result"]
            mainline = list(game.mainline_moves())
            for i in range(min(move_depth, len(mainline))):
                board_hash = chess.polyglot.zobrist_hash(board)
                if board_hash not in to_return.keys():
                    to_return[board_hash] = {}
                # Add the PGN if not in map
                next_move = mainline[i]
                if next_move not in to_return[board_hash].keys():
                    to_return[board_hash][next_move] = 0
                if result == DRAW:
                    to_return[board_hash][next_move] += DRAW_WEIGHT
                elif result == WHITE_VICTORY and board.turn == chess.WHITE:
                    to_return[board_hash][next_move] += VICTORY_WEIGHT
                elif result == BLACK_VICTORY and board.turn == chess.BLACK:
                    to_return[board_hash][next_move] += VICTORY_WEIGHT
                board.push(next_move)
    return to_return

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", default=6)
    args = parser.parse_args()

    pool = multiprocessing.Pool()
    files = [x for x in os.listdir(HERE_PATH) if os.path.isfile(os.path.join(HERE_PATH, x)) and os.path.splitext(x)[-1].lower() == ".pgn"]

    parsed_files: List[Dict[int, Dict[chess.Move, int]]] = list(tqdm.tqdm(pool.imap_unordered(read_file, zip(files, [int(args.n) for _ in range(len(files))])), total=len(files)))
    dct: Dict[int, Dict[chess.Move, int]] = {}

    for elem in parsed_files:
        for key in elem.keys():
            int_key = int(key)
            if int_key not in dct.keys():
                dct[int_key] = {}
            for move in elem[key].keys():
                if move not in dct[int_key].keys():
                    dct[int_key][move] = 0
                dct[int_key][move] += elem[key][move]

    sorted_keys = sorted(dct.keys())
    with open("output.bin", 'wb') as f:
        for board in sorted_keys:
            total = sum(dct[board].values())
            divide_factor = max(float(total) / 65535, 1)
            for move in dct[board].keys():
                f.write(board.to_bytes(length=8, byteorder="big", signed=False))
                mov = move_to_polyglot(move)
                f.write(mov.to_bytes(length=2, byteorder="big", signed=False))
                weight = int(math.floor(float(dct[board][move]) / divide_factor))
                f.write(weight.to_bytes(length=2, byteorder="big", signed=False))
                f.write(int(0).to_bytes(length=4, byteorder="big", signed=False))


if __name__ == "__main__":
    main()