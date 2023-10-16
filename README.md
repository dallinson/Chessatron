# Chessatron


## Results

Each version was evaluated in a series of 1000 games, where it was allowed to take up to one second to evaluate each move and no pondering was allowed.

How each engine performed against an engine that generates random moves

| Name | Wins | Draws | Losses | Notes |
|------|------|-------|--------|-------|
| Chessatron_v1-random | 24 | 975 | 1 | This is the source used to generate random moves |
| Chessatron_v2-time-limit | 699 | 301 | 0 | Depth search limit is 4 |
| Chessatron_v3-alpha-beta | 891 | 109 | 0 ||
| Chessatron_v4-alpha-beta-improved | 755 244 | 1 ||

How each engine performed against the previous version:

| Name | Wins | Draws | Losses | Notes |
|------|------|-------|--------|-------|
| Chessatron_v2-time-limit | 699 | 301 | 0 | The previous version generated random moves |
| Chessatron_v3-alpha-beta | 26 | 897 | 77 ||
| Chessatron_v4-alpha-beta-improved | 26 | 390 | 584 ||
