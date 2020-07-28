#include"Game.h"



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
  setAllowedMask();
  //boardPlaced += coordsToMask(0,0) + coordsToMask(0,1)+ coordsToMask(0,4) + coordsToMask(3,7)+ coordsToMask(7,4);

}

std::string Game::boardString(){
  std::string ans = "X|01234567";
  //have an allowedMask
  unsigned long long int pass = 1;
  unsigned long long int potential = allowedMask;
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
  //ans += "Turn:" + (xTurn?"X":"O");
  ans += "\nTurn:";
  if(xTurn) ans += "X";
  else ans += "O";
  return ans;
}

bool Game::test(unsigned long long int tst){
  unsigned long long foe = xTurn?boardPlaced - boardX: boardX;
  unsigned long long int origPos = tst; //coordsToMask(r,c);
  unsigned long long int tmp;

  //up
  tmp = (origPos >> 8) & foe;
  do{tmp >>= 8;}
  while(tmp && (tmp & foe) && (tmp & ~topEdgeMask));
  if(tmp & (boardPlaced - foe)) return true;

  //down
  tmp = (origPos << 8) & foe;
  do{tmp <<= 8;}
  while(tmp && (tmp & foe) && ~(tmp & bottomEdgeMask));
  if(tmp & (boardPlaced - foe)) return true;

  //left
  tmp = (origPos << 1) & foe;
  do{tmp <<= 1;}
  while(tmp && (tmp & foe) && ~(tmp & leftEdgeMask));
  if(tmp & (boardPlaced - foe)) return true;

  //right
  tmp = (origPos >> 1) & foe;
  do{tmp >>= 1;}
  while(tmp && (tmp & foe) && ~(tmp & rightEdgeMask));
  if(tmp & (boardPlaced - foe)) return true;

  //up-right
  tmp = (origPos >> 9) & foe;
  do{tmp >>= 9;}
  while(tmp && (tmp & foe) && (tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) return true;

  //down-left
  tmp = (origPos << 9) & foe;
  do{tmp <<= 9;}
  while(tmp && (tmp & foe) && ~(tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) return true;

  //left-up
  tmp = (origPos << 7) & foe;
  do{tmp <<= 7;}
  while(tmp && (tmp & foe) && ~(tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) return true;

  //right-up
  tmp = (origPos >> 7) & foe;
  do{tmp >>= 7;}
  while(tmp && (tmp & foe) && ~(tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) return true;

  return tmp & (boardPlaced - foe);

}
unsigned long long int Game::getAllowedMask(){
  return allowedMask;
}
void Game::setAllowedMask(){
  unsigned long long int foe = boardPlaced;
  if(xTurn) foe -= boardX; //the O squares
  else foe = boardX; //the X squares
  unsigned long long int ans = foe;

  ans |= ( (ans & ~rightEdgeMask) >> 1 ) | ( (ans & ~leftEdgeMask) << 1);
  ans |= ((ans & ~topEdgeMask) << 8) | ((ans & ~bottomEdgeMask) >> 8);

  ans &= (~boardPlaced);
  //ans = ~0ull;
  unsigned long long int pass = 1;
  for(int r = 0; r < 8; r++){
    for(int c = 0; c < 8; c++){
      if(pass & ans && !test(pass)) ans -= pass;
      pass <<= 1;
    }
  }
  allowedMask = ans;
}

void Game::move(unsigned long long int move){
  if(!(move & allowedMask)) return; //Don't make moves which are not allowed
  unsigned long long foe = xTurn?boardPlaced - boardX: boardX;
  unsigned long long int origPos = move; //coordsToMask(r,c);
  unsigned long long int tmp;

  boardPlaced += move;
  if(!xTurn) boardX += move;

  //up
  tmp = (origPos >> 8) & foe;
  do{tmp >>= 8;}
  while(tmp && (tmp & foe) && (tmp & ~topEdgeMask));
  if(tmp & (boardPlaced - foe)){
    while(tmp != origPos){
      tmp <<= 8;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }

  //down
  tmp = (origPos << 8) & foe;
  do{tmp <<= 8;}
  while(tmp && (tmp & foe) && ~(tmp & bottomEdgeMask));
  if(tmp & (boardPlaced - foe)){
    while(tmp != origPos){
      tmp >>= 8;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }
  //left
  tmp = (origPos << 1) & foe;
  do{tmp <<= 1;}
  while(tmp && (tmp & foe) && ~(tmp & leftEdgeMask));
  if(tmp & (boardPlaced - foe)){
    while(tmp != origPos){
      tmp >>= 1;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }

  //right
  tmp = (origPos >> 1) & foe;
  do{tmp >>= 1;}
  while(tmp && (tmp & foe) && ~(tmp & rightEdgeMask));
  if(tmp & (boardPlaced - foe)){
    while(tmp != origPos){
      tmp <<= 1;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }

  //up-right
  tmp = (origPos >> 9) & foe;
  do{tmp >>= 9;}
  while(tmp && (tmp & foe) && (tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) {
    while(tmp != origPos){
      tmp <<= 9;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }

  //down-left
  tmp = (origPos << 9) & foe;
  do{tmp <<= 9;}
  while(tmp && (tmp & foe) && ~(tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) {
    while(tmp != origPos){
      tmp >>= 9;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }

  //left-up
  tmp = (origPos << 7) & foe;
  do{tmp <<= 7;}
  while(tmp && (tmp & foe) && ~(tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) {
    while(tmp != origPos){
      tmp >>= 7;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }

  //right-up
  tmp = (origPos >> 7) & foe;
  do{tmp >>= 7;}
  while(tmp && (tmp & foe) && ~(tmp & ~midMask));
  if(tmp & (boardPlaced - foe)) {
    while(tmp != origPos){
      tmp <<= 7;
      if(xTurn) boardX += tmp;
      else boardX -= tmp;
    }
  }
  xTurn = !xTurn;
  setAllowedMask();
}
