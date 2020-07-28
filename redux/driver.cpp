#include"Game.h"
#include<iostream>
using namespace std;

int main(){
  Game mainGame;
  //cout << mainGame.boardString() << endl;
  //mainGame.move(coordsToMask(2,4));
  //cout << mainGame.boardString() << endl;
  int y,x;
  while(mainGame.getAllowedMask()){
    cout << mainGame.boardString() << endl;
    cout << "Enter Row:";
    cin >> y;
    cout << "Enter Col:";
    cin >> x;
    mainGame.move(coordsToMask(y,x));
    if(!mainGame.getAllowedMask()){
      mainGame.xTurn = !mainGame.xTurn;
      mainGame.setAllowedMask();
    }
  }


  return 0;
}
