#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>
void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}


//Calculates how hard the board favours X
double moveXPrediction(Board desk, bool xCalc, char depth, double alphaX, double betaX){
																												// alphaX is lowest guaranteed X for player X
																												// betaX is highest guaranteed X for O player
  if(depth == 0){ //BOTTOM LEVEL, alpha beta pruning irrelevant, valueOfBoard()
		double ans = desk.getXCount();
		if(desk.getMoves() < 59){
			if(desk.getSquare(0,0) == 3) ans += 1;
			if(desk.getSquare(0,0) == 1) ans -= 1;
			if(desk.getSquare(0,7) == 3) ans += 1;
			if(desk.getSquare(0,7) == 1) ans -= 1;
			if(desk.getSquare(7,0) == 3) ans += 1;
			if(desk.getSquare(7,0) == 1) ans -= 1;
			if(desk.getSquare(7,7) == 3) ans += 1;
			if(desk.getSquare(7,7) == 1) ans -= 1;
			for(int c = 2; c < 6; c++){
				if(desk.getSquare(0,c) == 3) ans += 0.5;
				if(desk.getSquare(7,c) == 1) ans -= 0.5;
			}
			for(int r = 2; r < 6; r++){
				if(desk.getSquare(r,0) == 3) ans += 0.5;
				if(desk.getSquare(r,7) == 1) ans -= 0.5;
			}


			/*
			if(!desk.getSquare(0,0)){
				if(desk.getSquare(2,2) == 3) ans += 0.25;
				if(desk.getSquare(2,2) == 1) ans -= 0.25;

				if(desk.getSquare(1,0) == 3) ans -= 0.25;
				if(desk.getSquare(1,0) == 1) ans += 0.25;
				if(desk.getSquare(0,1) == 3) ans -= 0.25;
				if(desk.getSquare(0,1) == 1) ans += 0.25;
				if(desk.getSquare(1,1) == 3) ans -= 0.25;
				if(desk.getSquare(1,1) == 1) ans += 0.25;


			}

			if(!desk.getSquare(0,7)){
				if(desk.getSquare(2,5) == 3) ans += 0.25;
				if(desk.getSquare(2,5) == 1) ans -= 0.25;

				if(desk.getSquare(1,7) == 3) ans -= 0.25;
				if(desk.getSquare(1,7) == 1) ans += 0.25;
				if(desk.getSquare(0,6) == 3) ans -= 0.25;
				if(desk.getSquare(0,6) == 1) ans += 0.25;
				if(desk.getSquare(1,6) == 3) ans -= 0.25;
				if(desk.getSquare(1,6) == 1) ans += 0.25;

			}


			if(!desk.getSquare(7,0)){
				if(desk.getSquare(5,3) == 3) ans += 0.25;
				if(desk.getSquare(5,3) == 1) ans -= 0.25;

				if(desk.getSquare(6,0) == 3) ans -= 0.25;
				if(desk.getSquare(6,0) == 1) ans += 0.25;
				if(desk.getSquare(6,1) == 3) ans -= 0.25;
				if(desk.getSquare(6,1) == 1) ans += 0.25;
				if(desk.getSquare(7,1) == 3) ans -= 0.25;
				if(desk.getSquare(7,1) == 1) ans += 0.25;

			}


			if(!desk.getSquare(7,7)){
				if(desk.getSquare(5,5) == 3) ans += 0.25;
				if(desk.getSquare(5,5) == 1) ans -= 0.25;

				if(desk.getSquare(6,6) == 3) ans -= 0.25;
				if(desk.getSquare(6,6) == 1) ans += 0.25;
				if(desk.getSquare(6,7) == 3) ans -= 0.25;
				if(desk.getSquare(6,7) == 1) ans += 0.25;
				if(desk.getSquare(7,6) == 3) ans -= 0.25;
				if(desk.getSquare(7,6) == 1) ans += 0.25;

			}
			*/

		}



		return ans;


	}


  else{ // alpha beta prune minmax

    bool maxim = !(xCalc == desk.getXTurn()); //Are we maximising or minimising?
    std::vector<char> options = desk.getAllowedMoves();
    std::vector<double> consequences;
		consequences.reserve(32);
    std::vector<Board> states;
		states.reserve(32);
    double result;
    if(maxim){
      result = 0;
			for(int i = 0, l = options.size(); i < l; i++){
				states.push_back(desk);
				states.back().move(options[i]);
				consequences.push_back(moveXPrediction(states.back(), xCalc, depth - 1, alphaX, betaX));
				if(consequences.back() > result) result = consequences.back();
				if(result > alphaX) alphaX = result;
				if(betaX < alphaX) break;
			}
			return result;
		}
		else{
			result = 64;
			for(int i = 0, l = options.size(); i < l; i++){
				states.push_back(desk);
				states.back().move(options[i]);
				consequences.push_back(moveXPrediction(states.back(), xCalc, depth - 1, alphaX, betaX));
				if(consequences.back() < result) result = consequences.back();
				if(result < betaX) betaX = result;
				if(betaX > alphaX) break;
			}
			return result;
		}


		/*
    for(int i = 0, l = options.size(); i < l; i++){
      states.push_back(desk);
      states.back().move(options[i]);
      consequences.push_back(moveXPrediction(states.back(), xCalc, time / l));
    }
    for(int i = 0, l = consequences.size(); i < l; i++){
      if(maxim && consequences[i] > result){
        result = consequences[i];
      }
      else if(!maxim && consequences[i] < result){
        result = consequences[i];
      }
    }
		*/
    return result;
  }
	return 578548765; //we should never get here anyway
}


char bestMove(Board desk, bool xCalc, char depth = 5){  //return the best move, as a coord
																																	//timeToEval is in nanoseconds
  std::vector<Board> createdSpaces;
	createdSpaces.reserve(32);
  std::vector<char> options = desk.getAllowedMoves();
  std::vector<double> xCountOutcome;
	xCountOutcome.reserve(32);
  int bestIndex = 0;
  bool maxim = !(xCalc == desk.getXTurn());
  for(int c = 0, l = options.size(); c < l; c++){
    createdSpaces.push_back(desk);
    createdSpaces.back().move(options[c]);
    xCountOutcome.push_back(moveXPrediction(createdSpaces.back(), xCalc, depth, -100.1, 100.1));
    if(maxim && xCountOutcome.back() > xCountOutcome[bestIndex]) bestIndex = c;
    else if(!maxim && xCountOutcome.back() < xCountOutcome[bestIndex]) bestIndex = c;
  }
  return options[bestIndex];
}

Game::Game(){
  skippedTurn = false;
  player1 = true;
}
bool Game::move(){
  std::cout << field.boardString() << '\n';
	//std::cout << "pos 33 = " << field.getSquare((char)(3),(char)(3)) << std::endl;
  options = field.getAllowedMoves();
  char move;
  int row;
  int col;
  if((options.size() == 0 && skippedTurn) || field.getMoves() == 64){
    std::cout << "Game Over\nX:" << (int)(field.getXCount()) << " O:"<< (int)(field.getOCount());
    return false;
  }
  else if(options.size() == 0){
		std::cout << "No moves doable\n";
    skippedTurn = true;
    field.changeSide();
		player1 = field.xMove();
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
          //Currently, it's bad
			/*
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
			*/
			auto start = std::chrono::steady_clock::now();
			field.move(bestMove(field,false, 5));
															//False as we calculate for O
			player1 = field.xMove();

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
	    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
			return true;
    }
  }
  return true;
}
