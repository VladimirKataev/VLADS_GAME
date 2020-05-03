#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>

#define CALCDEPTH 7


void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}

int incidences = 0; //used to analyse the number of boards analysed in AB tree

const
double startMask[64] = {5 ,0,3,2,2,3,0, 5, //Weight of spots by location at the start
  											0 ,0,1,1,1,1,0, 0,
											  3 ,1,2,1,1,2,1, 3,
											  2 ,1,1,1,1,1,1, 2,
											  2 ,1,1,1,1,1,1, 2,
											  3 ,1,2,1,1,2,1, 3,
											  0 ,0,1,1,1,1,0, 0,
											  5 ,0,3,2,2,3,0, 5
										};
/*
const double endMask[64] =
										 {1,1,1,1,1,1,1,1, //Weight of spots at the end
											1,1,1,1,1,1,1,1,
											1,1,1,1,1,1,1,1,
											1,1,1,1,1,1,1,1,
											1,1,1,1,1,1,1,1,
											1,1,1,1,1,1,1,1,
											1,1,1,1,1,1,1,1,
											1,1,1,1,1,1,1,1
										};
*/

double boardEval(Board desk){ //The higher, the more X-ish the board
	incidences++;
	/*
	std::vector<char> muvz = desk.getAllowedMoves();
	if(muvz.size() == 0) {desk.changeSide(); muvz = desk.getAllowedMoves();}//account for no-move scenarios

	if(muvz.size() == 0) //GO FOR THE KILL
		return (desk.getXCount() > desk.getOCount())?1000:-1000;
	*/
	double ans = 0.0;
	double impurities = 0.0;
	unsigned long long int boardPlaced = desk.boardPlaced;
	unsigned long long int boardX = desk.boardX;
	//unsigned long long int mask = 1ull;
	char pieces = desk.getMoves();
	double mask[64];
	for(int i  = 0; i < 64; i++)
		mask[i] = startMask[i];

	if(boardPlaced & 1ull){ //check the top left corner
		mask[0] = 2;
		mask[8] = 2;
		mask[9] = 2;
	}

	if(boardPlaced & (1ull << 7)){ //check the top right corner
		mask[6] = 2;
		mask[14] = 2;
		mask[15] = 2;
	}

	if(boardPlaced & (1ull << 56)){ //check the bottom left corner
		mask[48] = 2;
		mask[49] = 2;
		mask[57] = 2;
	}

	if(boardPlaced & (1ull << 63)){ //check the bottom right corner
		mask[63] = 2;
		mask[55] = 2;
		mask[62] = 2;
	}








	double maskPos;
	for(int m = 0; m < 64; m++){
		maskPos = (pieces < 64- CALCDEPTH )?startMask[m]:1.0; //Find out the weight of the individual spot at this time
		ans += (double)(boardPlaced & 1ull) * (-1 + 2*(int)(boardX & 1ull)) * maskPos;
		boardX >>= 1;
		boardPlaced >>= 1;
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
		std::vector<char> nudges = desk.getAllowedMoves(); //NOTE TO SELF, CAN BE OPTIMISED OUT
		int l = nudges.size();
		Board state; //No longer an array, shortens space by l
		double notableConsequence = maxing?alphaX:betaX;
		double analysis;
		for(int i = 0;  i < l && alphaX < betaX; i++){
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
    }
    else{ //------------------------------------------Enemy AI time
          //Currently, it's bad
			auto start = std::chrono::steady_clock::now();
			incidences = 0;
			move = bestMove(field,false, CALCDEPTH);
														//False as we calculate for O, not X
			field.move(move);
			std::cout << "AI moved "; charToString(move);
			player1 = field.xMove();

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
	    std::cout << "elapsed time: " << elapsed_seconds.count() << "s, analysing " << incidences<<" boards\n";
    }
		std::cout << "BoardValue = " << boardEval(field) << '\n';
  }
  return true;
}
