#include"Game.h"
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>
#include<limits>

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

const unsigned long long int fullmask = -1LL; //Evil floating point

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

	unsigned long long int boardPlacedDynamic = desk.boardPlaced;
	unsigned long long int boardXDynamic = desk.boardX;

	if(boardPlacedDynamic == fullmask){
		if(desk.getXCount() == 32) return 0;
		else if (desk.getXCount() > 32) return 99999.0;
		return -99999.0;
	}

	double ans = 0.0; // basic
	double factor = (80 - desk.getMoves())/16;
	unsigned long long int cornerMaskDynamic = cornerMask;
	unsigned long long int edgeMaskDynamic = edgeMask;
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

// BitCount quickest for args that have only a few 1's
static int bitCountSparse(uint64_t m) {
    int ret = 0;
    for(; m ; m &= (m-1))
        ret++;
    return ret;
}

// BitCount quickest for args that may have any number of 1's
static int bitCountDense(uint64_t m) {
    uint64_t acc = m & 0x0101010101010101ULL;
    for(int i=1; i<8; i++)
        acc += (0x0101010101010101ULL & (m >> i));
    acc += (acc >> 32);
    acc += (acc >> 16);
    acc += (acc >> 8);
    return (int)(acc & 255ull);
}



static uint64_t col(unsigned n) { return 0x0101010101010101ULL << n; }
static uint64_t row(unsigned n) { return 0xFFULL << (n << 3); }

uint64_t edges = row(0) | row(7) | col(0) | col(7);
uint64_t corners = (row(0) | row(7)) & (col(0) | col(7));

int sergEval(uint64_t x, uint64_t o) {
    //uint64_t x = b.x, o = b.o;
    //incidence++;
    int xCount=bitCountDense(x),
        oCount=bitCountDense(o);
    //histo[xCount+oCount]++;


    if( xCount+oCount == 64 ) { // Board is full
        if(xCount == oCount) return 0;
        return xCount > oCount ? 99999 : -99999;
    }
	  int gamePhaseFactor = (80-oCount-xCount)/16,
        xSides = bitCountSparse(x & edges),
        oSides = bitCountSparse(o & edges),
        xCorners = bitCountSparse(x & corners),
        oCorners = bitCountSparse(o & corners);

    return xCount - oCount +
        gamePhaseFactor * (xSides - oSides +
                           gamePhaseFactor * (xCorners - oCorners));
}


//sergBoard vladBoardToSergBoard




//Calculates how hard the board favours X
double moveXPrediction(Board desk, bool xCalc, char depth, double alphaX, double betaX){
																												// alphaX is lowest guaranteed X for player X ~Looking to make high
																												// betaX is highest guaranteed X for O player ~Looking to make low
  if(depth == 0){ //BOTTOM LEVEL, alpha beta pruning irrelevant, valueOfBoard()
		return boardEval(desk);
	}
  else{ // alpha beta prune minmax

		std::vector<char> nudges = desk.getAllowedMoves(); //NOTE TO SELF, CAN BE OPTIMISED OUT
		int l = nudges.size();
		if(l == 0){
			desk.changeSide();
		}
		nudges = desk.getAllowedMoves(); l = nudges.size();
		if(l == 0) return boardEval(desk);


		bool maxing = !(xCalc == desk.getXTurn());

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


void compareHeuristics(Board desk){
	int vladVal = boardEval(desk);
	//																			x																o
	int sergVal = sergEval(desk.boardPlaced & desk.boardX, desk.boardPlaced - desk.boardX );

	if(vladVal == sergVal) std::cout << "The sergey and the vlad algorithm agree\n";
	else std::cout << "The 2 disagree, Vlad : " << vladVal << " Sergey : " << sergVal << '\n';
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
		compareHeuristics(field);
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
