# VLADS_GAME
Reversi, but with self-made AI

move into the /source directory
compile with "gcc -std=c++11 -pthread -o execName *.cpp *.h"

run it on Mac/Linux with ./execName. Windows isn't cash money

add a -DDEFAULT_DEPTH=12 to make default depth 12 (could be any positive number)
add a -DDYNAMIC_DEPTH to make the depth change if it's taking too long to calc
