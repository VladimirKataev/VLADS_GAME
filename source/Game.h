#include"Board.h"
#include<vector>
class Game{
public:
  Board field;
  bool player1;
  bool skippedTurn;
  std::vector<char> options;
  Game();
  bool move(); //the bool is whether the game continuess
};
