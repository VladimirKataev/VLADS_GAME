#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>

#define CALCDEPTH 6


void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}


double boardEval(Board desk){ //The higher, the more X-ish the board

	if(desk.getAllowedMoves().size() == 0) desk.changeSide(); //account for no-move scenarios

	if(desk.getAllowedMoves().size() == 0) //GO FOR THE KILL
		return (desk.getXCount() > desk.getOCount())?1000:-1000;

	double ans = 0.0 + desk.getXCount();
	double impurities = 0.0;
	char pieces = desk.getMoves();

	if(pieces < (64 - CALCDEPTH)){ //While heuristics are still important



		//PER EDGE: edge counts for +1, as do the 2 ortho neighbours, as unconvertible
		//note, if edge not taken, don't go neighbour
		//----------------------------------------------------------------------------


		char 	tl = desk.getSquare(0,0),
					tr = desk.getSquare(0,7),
					bl = desk.getSquare(7,0),
					br = desk.getSquare(7,7),

					tlr = desk.getSquare(0,1),
					tlb = desk.getSquare(1,0),
					tld = desk.getSquare(1,1),

					trl = desk.getSquare(0,6),
					trb = desk.getSquare(1,7),
					trd = desk.getSquare(1,6),

					blr = desk.getSquare(7,1),
					blt = desk.getSquare(6,0),
					bld = desk.getSquare(6,1),

					brl = desk.getSquare(7,6),
					brt = desk.getSquare(6,7),
					brd = desk.getSquare(6,6);
		if(tl){
			impurities += (tl == 3)?2:-2;
			if(tlb == tl){
				impurities += (tl==3)?1:-1;
			}
			if(tlr == tl){
				impurities += (tl==3)?1:-1;
			}
		}
		else{
			if(tlb){
				impurities += (tlb==1)?1:-1;
			}
			if(tlr){
				impurities += (tlr==1)?1:-1;
			}
			if(tld){
				impurities += (tld == 1)?1:-1;
			}
		}
		if(tr){
			impurities += (tr == 3)?1:-1;
			if(trb == tr){
				impurities += (trb==3)?1:-1;
			}
			if(trl == tr){
				impurities += (trl==3)?1:-1;
			}
		}
		else{
			if(trb){
				impurities += (trb==1)?1:-1;
			}
			if(trl){
				impurities += (trl==1)?1:-1;
			}
			if(trd){
				impurities += (trd == 1)?1:-1;
			}
		}
		if(bl){
			impurities += (blr == 3)?1:-1;
			if(blr == bl){
				impurities += (blr==3)?1:-1;
			}
			if(blt == bl){
				impurities += (blt==3)?1:-1;
			}
		}
		else{
			if(blr){
				impurities += (blr==1)?1:-1;
			}
			if(blt){
				impurities += (blt==1)?1:-1;
			}
			if(bld){
				impurities += (tld == 1)?1:-1;
			}

		}

		if(br){
			impurities += (brt == 3)?1:-1;
			if(brt == br){
				impurities += (trb==3)?1:-1;
			}
			if(brl == br){
				impurities += (brl==3)?1:-1;
			}
		}
		else{
			if(brt){
				impurities += (brt==1)?1:-1;
			}
			if(brl){
				impurities += (brl==1)?1:-1;
			}
			if(brd){
				impurities += (brd == 1)?1:-1;
			}

		}
		//Total getSquare calls for entire board: 80
		//vs previous 120
		//Now to do it with no if elses=
	}

	return ans + (impurities * ((64 - pieces) / (64 - CALCDEPTH)));
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
		std::vector<char> nudges = desk.getAllowedMoves(); //NOTE TO SELF, CAN BE OPTIMISED OUT
		int l = nudges.size();
		Board state; //No longer an array, shortens space by l
		double notableConsequence = maxing?alphaX:betaX;
		double analysis;
		for(int i = 0;  i < l && alphaX < betaX; i++){ // NEEDS A FIX

			if(maxing){ // Attempt to raise alpha
					state = desk;
					state.move(nudges[i]);
					analysis = moveXPrediction(state, xCalc, depth-1, alphaX, betaX);
					alphaX = (analysis > alphaX)?analysis:alphaX;
					notableConsequence = alphaX;
			}
			else{
				state = desk;
				state.move(nudges[i]);
				analysis = moveXPrediction(state, xCalc, depth-1, alphaX, betaX);
				betaX = (analysis < betaX)?analysis:betaX;
				notableConsequence = betaX;
			}

		}

		return notableConsequence;

  }
}


char bestMove(Board desk, bool xCalc, char depth = 5){  //return the best move, as a coord
																																	//timeToEval is in nanoseconds
  std::vector<char> options = desk.getAllowedMoves();
	int l = options.size();
	//Board createdSpaces[l];
	Board createdSpace; //No longer an array, shortens space by l
	//createdSpaces.reserve(32);
  double xCountOutcome[l];
	//xCountOutcome.reserve(32);
  int bestIndex = 0;
  bool maxim = !(xCalc == desk.getXTurn());
	double alpha = -99999.0;
	double beta = 99999.0;
  for(int c = 0; c < l; c++){
    createdSpace = desk;
    createdSpace.move(options[c]);
    xCountOutcome[c] = (moveXPrediction(createdSpace, xCalc, depth, alpha, beta));
    if(maxim && xCountOutcome[c] > xCountOutcome[bestIndex]){
			bestIndex = c;
			alpha = xCountOutcome[c];
		}
    else if(!maxim && xCountOutcome[c] < xCountOutcome[bestIndex]){
			bestIndex = c;
			beta = xCountOutcome[c];
		}
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
			move = bestMove(field,false, CALCDEPTH);
														//False as we calculate for O, not X
			field.move(move);
			std::cout << "AI moved "; charToString(move);
			player1 = field.xMove();

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
	    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
			return true;
    }
  }
  return true;
}
