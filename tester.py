import chess
import subprocess

def perft(board: chess.Board, depth):
    if depth < 1:
        return
    lmg = subprocess.Popen("/mnt/d/Projects/chessatron-linux/build/Chessatron_legal_move_gen", shell=True, text=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.DEVNULL)
    lmg.stdin.write("ucinewgame\n")
    lmg.stdin.write("position fen " + str(board.fen()) + "\n")
    lmg.stdin.write("go perft 1\n")
    lmg_perft = lmg.communicate()[0]

    plmg = subprocess.Popen("/mnt/d/Projects/chessatron-linux/build/Chessatron_pseudolegal_move_gen", shell=True, text=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.DEVNULL)
    plmg.stdin.write("ucinewgame\n")
    plmg.stdin.write("position fen " + str(board.fen()) + "\n")
    plmg.stdin.write("go perft 1\n")
    plmg_perft = lmg.communicate()[0]
    #print(lmg.communicate("ucinewgame\nposition fen " + str(board.fen()) + "\ngo perft 1\nquit"))

    if (plmg_perft != lmg_perft):
        print(board.fen())
    else:
        #print(plmg_perft, lmg_perft)
        pass
    lmg.kill()
    plmg.kill()
    
    for move in board.legal_moves:
        board.push(move)
        perft(board, depth - 1)
        board.pop()

b = chess.Board()
b.set_fen(chess.STARTING_FEN)
perft(b, 4)