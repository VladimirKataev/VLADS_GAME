#include"AI.h"
#include<string>
class Game{
public:
  unsigned long long int boardPlaced;
  unsigned long long int boardX;
  bool xTurn;
  Game();
  std::string boardString();
  unsigned long long int allowedMask(); //finished
  bool test(int r, int c);//return true if allowed to move to rc
  char testDir(char pos, char dir);
  //void move();
};
