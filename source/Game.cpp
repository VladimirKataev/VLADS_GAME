#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>


void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}

int incidences = 0; //used to analyse the number of boards analysed in AB tree

/*
const double startMask[64] = {5 ,-1,3,2,2,3,-1, 5, //Weight of spots by location at the start
  											-1,-1,1,1,1,1,-1,-1,
											  3 ,1,2,1,1,2,1, 3,
											  2 ,1,1,1,1,1,1, 2,
											  2 ,1,1,1,1,1,1, 2,
											  3 ,1,2,1,1,2,1, 3,
											  -1,-1,1,1,1,1,-1,-1,
											  5 ,-1,3,2,2,3,-1, 5
										};
*/

const unsigned long long int cornerMask =
	(1ull) + (1ull << 7) + (1ull << 56) + (1ull << 63);

const unsigned long long int edgeMask =
	cornerMask +
	(1ull << 1) + (1ull << 2) + (1ull << 3) + (1ull << 4) + (1ull << 5) + (1ull << 6)
	+ (1ull << 8)+ 	(1ull << 15)
	+ (1ull << 16)+  (1ull << 23)
	+ (1ull << 24)+   (1ull << 31)
	+ (1ull << 32)+   (1ull << 39)
	+ (1ull << 40)+   (1ull << 47)
	+ (1ull << 48)+   (1ull << 55)
	+ (1ull << 57) + (1ull << 58) + (1ull << 59) + (1ull << 60) + (1ull << 61) + (1ull << 62);

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
	incidences++; // arghh dev stuffs
	/*
	std::vector<char> muvz = desk.getAllowedMoves();
	if(muvz.size() == 0) {desk.changeSide(); muvz = desk.getAllowedMoves();}//account for no-move scenarios

	if(muvz.size() == 0) //GO FOR THE KILL
		return (desk.getXCount() > desk.getOCount())?1000:-1000;
	*/
	//int xCount = desk.getXCount();
	//int oCount = desk.getOCount();

	double ans = 0.0; // basic
	double factor = (80 - desk.getMoves())/16;
	unsigned long long int cornerMaskDynamic = cornerMask;
	unsigned long long int edgeMaskDynamic = edgeMask;
	unsigned long long int boardPlacedDynamic = desk.boardPlaced;
	unsigned long long int boardXDynamic = desk.boardX;
	double tmp = 0.0;
	while(cornerMaskDynamic){

		tmp = ((boardPlacedDynamic & 1ull) * (-1.0 + 2.0*(boardXDynamic & 1ull))); //-1, 0, or 1
		tmp *= 1.0 + (factor * (edgeMaskDynamic & 1ull)) ; //Decide if corner factor applies or not
		tmp *= 1.0 + (factor * (cornerMaskDynamic & 1ull)); //

		ans += tmp;

		cornerMaskDynamic >>= 1ull;
		edgeMaskDynamic >>= 1ull;
		boardPlacedDynamic >>= 1ull;
		boardXDynamic >>= 1ull;
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


int thinkfast = 7; //How many moves ahead AI thinks

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
			move = bestMove(field,false, thinkfast);
														//False as we calculate for O, not X
			field.move(move);
			std::cout << "AI moved "; charToString(move);
			player1 = field.xMove();

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
	    std::cout << "elapsed time: " << elapsed_seconds.count() << "s, analysing " << incidences<<" boards to " << thinkfast <<" moves ahead\n";

			if(elapsed_seconds.count() < 0.1) thinkfast++;
			else if (elapsed_seconds.count() > 0.25) thinkfast--;

    }
		std::cout << "BoardValue = " << boardEval(field) << '\n';
  }
  return true;
}
