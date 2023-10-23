import os

HERE_PATH = os.path.dirname(os.path.abspath(__file__))
files = [x for x in os.listdir(HERE_PATH) if os.path.isfile(os.path.join(HERE_PATH, x)) and os.path.splitext(x)[-1].lower() == ".pgn"]
for filename in files:
    print(filename)
    lines = ""
    with open(os.path.join(HERE_PATH, filename), 'r') as f:
        lines = f.read()
    with open(os.path.join(HERE_PATH, filename), 'w', encoding='utf8') as f:
        f.write(lines.replace("]\n\n", "]\n"))
