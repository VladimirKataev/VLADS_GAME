#include"AI.h"
#include<string>
#define coordsToMask(r, c) (1ull << ((r * 8) + c))

class Game{
public:
  unsigned long long int boardPlaced;
  unsigned long long int boardX;
  bool xTurn;
  unsigned long long int allowedMask; //preemptive memoisation to prevent recalc
  Game();
  std::string boardString();
  unsigned long long int getAllowedMask(); //returns allowedMask
  bool test(unsigned long long int);//return true if allowed to move to rc
  char testDir(char pos, char dir);
  void setAllowedMask(); //Purely internal, to be calculated as few as possible
  void move(unsigned long long int); //NOTE RECALCS ALLOWEDMASK
};
