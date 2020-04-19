#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>

void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}


double boardEval(Board desk){ //The higher, the more X-ish the board
	double ans = 0.0 + desk.getXCount();

	if(desk.getAllowedMoves().size() == 0) desk.changeSide(); //account for no-move scenarios

	if(desk.getAllowedMoves().size() == 0) //GO FOR THE KILL
		return (desk.getXCount() > desk.getOCount())?1000:-1000;



	if(desk.getMoves() < 59){ //While heuristics are still important


		//PER EDGE: edge counts for +1, as do the 2 ortho neighbours, as unconvertible
		//note, if edge not taken, don't go neighbour
		//----------------------------------------------------------------------------
		if(desk.getSquare(0,0)){
			ans += (desk.getSquare(0,0) == 3)?1:-1;
			if(desk.getSquare(1,0) == desk.getSquare(0,0)){
				ans += (desk.getSquare(0,0)==3)?1:-1;
			}
			if(desk.getSquare(0,1) == desk.getSquare(0,0)){
				ans += (desk.getSquare(0,0)==3)?1:-1;
			}
		}
		else{
			if(desk.getSquare(1,0)){
				ans += (desk.getSquare(1,0)==1)?1:-1;
			}
			if(desk.getSquare(0,1)){
				ans += (desk.getSquare(0,1)==1)?1:-1;
			}
			if(desk.getSquare(1,1)){
				ans += (desk.getSquare(1,1) == 1)?1:-1;
			}
		}



		if(desk.getSquare(0,7)){
			ans += (desk.getSquare(0,7) == 3)?1:-1;
			if(desk.getSquare(1,7) == desk.getSquare(0,7)){
				ans += (desk.getSquare(1,7)==3)?1:-1;
			}
			if(desk.getSquare(0,6) == desk.getSquare(0,7)){
				ans += (desk.getSquare(0,6)==3)?1:-1;
			}
		}
		else{
			if(desk.getSquare(1,7)){
				ans += (desk.getSquare(1,7)==1)?1:-1;
			}
			if(desk.getSquare(0,6)){
				ans += (desk.getSquare(0,6)==1)?1:-1;
			}
			if(desk.getSquare(1,6)){
				ans += (desk.getSquare(1,6) == 1)?1:-1;
			}

		}


		if(desk.getSquare(7,0)){
			ans += (desk.getSquare(7,1) == 3)?1:-1;
			if(desk.getSquare(7,1) == desk.getSquare(7,0)){
				ans += (desk.getSquare(7,1)==3)?1:-1;
			}
			if(desk.getSquare(6,0) == desk.getSquare(7,0)){
				ans += (desk.getSquare(6,0)==3)?1:-1;
			}
		}
		else{
			if(desk.getSquare(7,1)){
				ans += (desk.getSquare(7,1)==1)?1:-1;
			}
			if(desk.getSquare(6,0)){
				ans += (desk.getSquare(6,0)==1)?1:-1;
			}
			if(desk.getSquare(6,1)){
				ans += (desk.getSquare(1,1) == 1)?1:-1;
			}

		}

		if(desk.getSquare(7,7)){
			ans += (desk.getSquare(6,7) == 3)?1:-1;
			if(desk.getSquare(6,7) == desk.getSquare(7,7)){
				ans += (desk.getSquare(1,7)==3)?1:-1;
			}
			if(desk.getSquare(7,6) == desk.getSquare(7,7)){
				ans += (desk.getSquare(7,6)==3)?1:-1;
			}
		}
		else{
			if(desk.getSquare(6,7)){
				ans += (desk.getSquare(6,7)==1)?1:-1;
			}
			if(desk.getSquare(7,6)){
				ans += (desk.getSquare(7,6)==1)?1:-1;
			}
			if(desk.getSquare(6,6)){
				ans += (desk.getSquare(6,6) == 1)?1:-1;
			}

		}
		//UN-Per EDGE
		//-----------------------------
	}

	return ans;
}


//Calculates how hard the board favours X
double moveXPrediction(Board desk, bool xCalc, char depth, double alphaX, double betaX){
																												// alphaX is lowest guaranteed X for player X ~Looking to make high
																												// betaX is highest guaranteed X for O player ~Looking to make low

  if(depth == 0){ //BOTTOM LEVEL, alpha beta pruning irrelevant, valueOfBoard()
		return boardEval(desk);
	}


  else{ // alpha beta prune minmax
		bool maxing = !(xCalc == desk.getXTurn());
		std::vector<char> nudges = desk.getAllowedMoves();
		int l = nudges.size();
		Board states[l];
		double consequences[l];
		for(int i = 0;  i < l && betaX > alphaX; i++){ // for each possible move
			if(maxing){ //we want highest alpha
				states[i] = (desk);
				states[i].move(nudges[i]);
				consequences[i] = (moveXPrediction(states[i], xCalc, depth -1, alphaX, betaX));
				if(consequences[i] > alphaX) alphaX = consequences[i];
			}
			else{//we want lowest beta
				states[i] = desk;
				states[i].move(nudges[i]);
				consequences[i] = moveXPrediction(states[i], xCalc, depth -1, alphaX, betaX);
				if(consequences[i] < betaX) betaX = consequences[i];
			}
		}

		return maxing?alphaX:betaX;

  }
}


char bestMove(Board desk, bool xCalc, char depth = 5){  //return the best move, as a coord
																																	//timeToEval is in nanoseconds
  std::vector<char> options = desk.getAllowedMoves();
	int l = options.size();
	Board createdSpaces[l];
	//createdSpaces.reserve(32);
  double xCountOutcome[l];
	//xCountOutcome.reserve(32);
  int bestIndex = 0;
  bool maxim = !(xCalc == desk.getXTurn());
  for(int c = 0; c < l; c++){
    createdSpaces[c] = desk;
    createdSpaces[c].move(options[c]);
    xCountOutcome[c] = (moveXPrediction(createdSpaces[c], xCalc, depth, -100.1, 100.1));
    if(maxim && xCountOutcome[c] > xCountOutcome[bestIndex]) bestIndex = c;
    else if(!maxim && xCountOutcome[c] < xCountOutcome[bestIndex]) bestIndex = c;
  }
  return options[bestIndex];
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
			auto start = std::chrono::steady_clock::now();
			field.move(bestMove(field,false, 4));
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
