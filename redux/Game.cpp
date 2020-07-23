#include"Game.h"

#define coordsToMask(r, c) (1ull << ((r * 8) + c))


unsigned long long int midMask =
0b00000000\
01111110\
01111110\
01111110\
01111110\
01111110\
01111110\
00000000;

unsigned long long int cornerMask =
0b10000001\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
10000001;

unsigned long long int edgeMask = (~midMask) - cornerMask;

unsigned long long int rightEdgeMask =
0b00000001\
00000001\
00000001\
00000001\
00000001\
00000001\
00000001\
00000001;

unsigned long long int leftEdgeMask  = rightEdgeMask << 7;

unsigned long long int topEdgeMask =
0b11111111\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000;

unsigned long long int bottomEdgeMask = topEdgeMask >> 56;


Game::Game(){
  boardX = 0;
  boardPlaced = 0;
  boardPlaced += coordsToMask(3,3) + coordsToMask(3,4) + coordsToMask(4,4) + coordsToMask(4,3);
  boardX += coordsToMask(3,3) + coordsToMask(4,4);
  xTurn = true;

  //boardPlaced += coordsToMask(0,0) + coordsToMask(0,1)+ coordsToMask(0,4) + coordsToMask(3,7)+ coordsToMask(7,4);

}

std::string Game::boardString(){
  std::string ans = "X|01234567";
  //have an allowedMask
  unsigned long long int pass = 1;
  unsigned long long int potential = allowedMask();
  for(int r = 0; r < 8; r++){
    ans += "\n" + std::to_string(r) + "|";
    for(int c = 0; c < 8; c++){
      if(pass & boardPlaced){
        if(pass & boardX) ans += "X";
        else ans += "O";
      }
      else if(pass & potential) ans += "?";
      else ans += "-";
      pass <<= 1;
    }
  }
  ans += "Turn:" + (xTurn?"X":"O");
  return ans;
}

bool Game::test(int r, int c){
  bool foe = !xTurn;
  unsigned long long int origPos = coordsToMask(r,c);
  char oldMovePos = c + (r << 4);
  return false;
}



unsigned long long int Game::allowedMask(){
  unsigned long long int foe = boardPlaced;
  if(xTurn) foe -= boardX; //the O squares
  else foe = boardX; //the X squares
  unsigned long long int ans = foe;

  ans |= ( (ans & ~rightEdgeMask) >> 1 ) | ( (ans & ~leftEdgeMask) << 1);
  ans |= ((ans & ~topEdgeMask) << 8) | ((ans & ~bottomEdgeMask) >> 8);

  ans &= (~boardPlaced);
  unsigned long long int pass = 1;
  for(int r = 0; r < 7; r++){
    for(int c = 0; c < 7; c++){
      if(pass & ans && !test(r,c)) ans -= pass;
      pass <<= 1;
    }
  }
  return ans;
}
