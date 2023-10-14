import chess
import chess.engine
import argparse
import multiprocessing
import tqdm
from typing import Tuple
import logging

def evaluate_board(args: Tuple[str, str, int]):
    first_engine = chess.engine.SimpleEngine.popen_uci(args[0])
    second_engine = chess.engine.SimpleEngine.popen_uci(args[1])
    board = chess.Board()
    board.set_fen(board.starting_fen)

    while not board.is_game_over():
        result = first_engine.play(board, limit = chess.engine.Limit(time=1))
        board.push(result.move or chess.Move.from_uci("0000"))
        if board.is_game_over():
            break
        result = second_engine.play(board, limit = chess.engine.Limit(time=1))
        board.push(result.move or chess.Move.from_uci("0000"))

    first_engine.quit()
    second_engine.quit()

    outcome = board.outcome()
    if outcome.winner is None:
        # it's a draw
        return 0
    else:
        if outcome.winner:
            # this is a white victory
            return 1 * args[2]
        else:
            return -1 * args[2]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--count", default=1000)
    parser.add_argument("-f", "--first", required=True)
    parser.add_argument("-s", "--second", required=True)
    args = parser.parse_args()

    first_wins = 0
    second_wins = 0
    draws = 0

    pool = multiprocessing.Pool()

    if (int(args.count) % 2 == 0):
        first_lst = [args.first for _ in range(int(int(args.count) / 2))]
        first_lst.extend([args.second for _ in range(int(int(args.count) / 2))])
        second_lst = [args.second for _ in range(int(int(args.count) / 2))]
        second_lst.extend([args.first for _ in range(int(int(args.count) / 2))])
        third_lst = [1 for _ in range(int(int(args.count) / 2))]
        third_lst.extend([-1 for _ in range(int(int(args.count) / 2))])
        zipped = list(zip(first_lst, second_lst, third_lst))
        res = list(tqdm.tqdm(pool.imap_unordered(evaluate_board, zipped), total=int(args.count)))
        first_wins = res.count(1)
        second_wins = res.count(-1)
        draws = res.count(0)

    print("Wins for first engine:", first_wins)
    print("Wins for second engine:", second_wins)
    print("Draws:", draws)

if __name__ == "__main__":
    main()