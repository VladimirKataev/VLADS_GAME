#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}
Game::Game(){
  skippedTurn = false;
  player1 = true;
}
bool Game::move(){
  std::cout << field.boardString() << '\n';
  options = field.getAllowedMoves();
  char move;
  int row;
  int col;
  if((options.size() == 0 && skippedTurn) || field.getMoves() == 64){
    std::cout << "Game Over\nX:" << field.getXCount() << " O:"<<field.getOCount();
    std::cout << "exit 1\n";
    return false;
  }
  else if(options.size() == 0){
    skippedTurn = true;
    player1 = field.xMove();
    field.changeSide();
    return true;
  }
  else{
    skippedTurn = false;
    for_each(begin(options), end(options), charToString);
    std::cout << '\n';
    if(player1){
      do{
        std::cout << "Enter row:";
        std::cin >> row;
      }
      while(0 > row || row > 8);
      do{
        std::cout << "Enter col:";
        std::cin >> col;
      }
      while(0 > col || col > 8);
      move = 0;
      move += col;
      move += (row << 4);
      field.move(move);
      player1 = field.xMove();
      return true;
    }
    else{ //------------------------------------------Enemy AI time
      char preference = 0;
      for(int i = 0, l = options.size(); i < l; i++){
        if(field.allowedMove(options[i]) > preference){
          preference = field.allowedMove(options[i]);
          move = options[i];
        }
      }
      field.move(move);
      player1 = field.xMove();
      return true;
    }
  }
  return true;
}
