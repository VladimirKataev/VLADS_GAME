#include"Game.h"
#include<stdlib.h>
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>
#include<limits>
#include<future>
void charToString(char in){
	std::cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
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




int incidences = 0; //used to analyse the number of boards analysed in AB tree

/* //Remnant of past heuristics
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

const unsigned long long int antiEdgeMask =
	cornerMask +
	/*(1ull << 1)*/ + (1ull << 2) + (1ull << 3) + (1ull << 4) + (1ull << 5) + /*(1ull << 6)*/
	//+ (1ull << 8) + 	(1ull << 15)
	+ (1ull << 16)+  (1ull << 23)
	+ (1ull << 24)+   (1ull << 31)
	+ (1ull << 32)+   (1ull << 39)
	+ (1ull << 40)+   (1ull << 47)
	//+ (1ull << 48)+   (1ull << 55)
	+ /*(1ull << 57)*/ + (1ull << 58) + (1ull << 59) + (1ull << 60) + (1ull << 61) /*+ (1ull << 62)*/;






const unsigned long long int fullmask = -1LL; //Evil floating point

/* //Remnant of past heuristics
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
	//incidences++; // arghh dev stuffs
	/*
	std::vector<char> muvz = desk.getAllowedMoves(); //Evaluated at wrong parts
	if(muvz.size() == 0) {desk.changeSide(); muvz = desk.getAllowedMoves();}//account for no-move scenarios

	if(muvz.size() == 0) //GO FOR THE KILL
		return (desk.getXCount() > desk.getOCount())?1000:-1000;
	*/


	//int xCount = desk.getXCount(); //Removed because has 0(64) runtime. as had the second one
	//int oCount = desk.getOCount();

	unsigned long long int boardPlacedDynamic = desk.boardPlaced;
	unsigned long long int boardXDynamic = desk.boardX;

	if(boardPlacedDynamic == fullmask){
		if(desk.getXCount() == 32) return 0;
		else if (desk.getXCount() > 32) return 99999.0;
		return -99999.0;
	}

	double ans = 0.0; // basic
	int factor = (80 - bitCountDense(desk.boardPlaced))/16;
	unsigned long long int cornerMaskDynamic = cornerMask;
	unsigned long long int edgeMaskDynamic = edgeMask;
	double tmp = 0.0;
	while(boardPlacedDynamic){

		tmp = ((boardPlacedDynamic & 1ull) * (-1.0 + 2.0*(boardXDynamic & 1ull))); //-1, 0, or 1

		//if(edgeMaskDynamic & 1ull) tmp *= factor;
		//if(cornerMaskDynamic & 1ull) tmp *= factor;
		//tmp *= 1.0 + (factor * (edgeMaskDynamic & 1ull)) ; //Decide if corner factor applies or not
		//tmp *= 1.0 + (factor * (cornerMaskDynamic & 1ull)); //


		tmp += tmp * (factor * ((edgeMaskDynamic & 1ull) + (factor * (cornerMaskDynamic & 1ull)))); //dad ficed it for me, yay

		ans += tmp;

		cornerMaskDynamic >>= 1ull;
		edgeMaskDynamic >>= 1ull;
		boardPlacedDynamic >>= 1ull;
		boardXDynamic >>= 1ull;
	}
	return ans;
}

double newBoardEval(Board desk){

	unsigned long long int boardPlacedDynamic = desk.boardPlaced;
	unsigned long long int boardXDynamic = desk.boardX;

	if(boardPlacedDynamic == fullmask){
		if(desk.getXCount() == 32) return 0;
		else if (desk.getXCount() > 32) return 99999.0;
		return -99999.0;
	}

	double ans = 0.0; // basic
	int factor = (80 - bitCountDense(desk.boardPlaced))/16;
	unsigned long long int cornerMaskDynamic = cornerMask;
	unsigned long long int edgeMaskDynamic = antiEdgeMask;
	double tmp = 0.0;
	while(boardPlacedDynamic){

		tmp = ((boardPlacedDynamic & 1ull) * (-1.0 + 2.0*(boardXDynamic & 1ull))); //-1, 0, or 1

		//if(edgeMaskDynamic & 1ull) tmp *= factor;
		//if(cornerMaskDynamic & 1ull) tmp *= factor;
		//tmp *= 1.0 + (factor * (edgeMaskDynamic & 1ull)) ; //Decide if corner factor applies or not
		//tmp *= 1.0 + (factor * (cornerMaskDynamic & 1ull)); //


		tmp += tmp * (factor * ((edgeMaskDynamic & 1ull) + (factor * (cornerMaskDynamic & 1ull)))); //dad ficed it for me, yay

		ans += tmp;

		cornerMaskDynamic >>= 1ull;
		edgeMaskDynamic >>= 1ull;
		boardPlacedDynamic >>= 1ull;
		boardXDynamic >>= 1ull;
	}
	return ans;
}


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



//Alpha-beta pruner, with numbers
//Calculates how hard the board favours X, at depth 0 using heuristic
double moveXPrediction(Board desk, bool xCalc, char depth, double alphaX, double betaX, double (*heuristic)(Board) = boardEval){
																												// alphaX is lowest guaranteed X for player X ~Looking to make high
																												// betaX is highest guaranteed X for O player ~Looking to make low
  if(depth == 0){ //BOTTOM LEVEL, alpha beta pruning irrelevant, valueOfBoard()
		return heuristic(desk);
	}
  else{ // alpha beta prune minmax

		int l = 0; //nudges.size();
		/*
		*/

		bool maxing = !(xCalc == desk.getXTurn());

		Board state; //No longer an array, shortens space by l
		double notableConsequence = maxing?alphaX:betaX;
		double analysis;

		std::vector<char> nudges = desk.getAllowedMoves(); //NOTE TO SELF, CAN BE OPTIMISED OUT
		l = nudges.size();
		if(l == 0){
			desk.changeSide();
			nudges = desk.getAllowedMoves(); l = nudges.size();
		}
		if(l == 0) return heuristic(desk);
		for(int i = 0;  i < l && alphaX < betaX; i++){
			state = desk;
			state.move(nudges[i]);
			analysis = moveXPrediction(state, xCalc, depth-1, alphaX, betaX);

			if(maxing){ // Attempt to raise alpha
					alphaX = (analysis > alphaX)?analysis:alphaX;
					notableConsequence = alphaX;
			}
			else{
				betaX = (analysis < betaX)?analysis:betaX;
				notableConsequence = betaX;
			}

		}


		return notableConsequence;

  }
}


//generic alpha-beta pruner now
//							Where we evalling from	Depth 						function for evaluating board
char bestMove(Board desk, bool xCalc, char depth = 5, double (*heuristic)(Board) = boardEval){  //return the best move, as a coord
																																	//timeToEval is in nanoseconds
  std::vector<char> options = desk.getAllowedMoves();
	int l = options.size();
	//Board createdSpaces[l];
	Board createdSpace; //No longer an array, shortens space by l
	//createdSpaces.reserve(32);
  std::future<double> xCountOutcome[l]; //-pthread option STRICTLY NEEDED
	double results[l];
	//xCountOutcome.reserve(32);
  int bestIndex = 0;
  bool maxim = !(xCalc == desk.getXTurn());
	double alpha = -99999.0;
	double beta = 99999.0;
  for(int c = 0; c < l; c++){
    createdSpace = desk;
    createdSpace.move(options[c]);
    xCountOutcome[c] = std::async(moveXPrediction,createdSpace, xCalc, depth, alpha, beta, heuristic);
	}

	//Wait for the xCountOutcome futures to be done
	for(int c= 0; c < l; c++){
		results[c] = xCountOutcome[c].get();
	}


	//Pick the best index by maxing | minning
	for(int c = 0; c < l; c++){
	  if(maxim && results[c] > results[bestIndex]){
			bestIndex = c;
			alpha = results[c];
		}
	  else if(!maxim && results[c] < results[bestIndex]){
			bestIndex = c;
			beta = results[c];
		}
	}

  return options[bestIndex];
}

//ruins screen with mask printout
//Remnant of debugging
void printMask(unsigned long long int in){
	unsigned long long int meta = (1ull << 63);
	unsigned long long int metameta = 1;
	std::cout << "Mask :";
	while(metameta){
		if(in & metameta) std::cout << "1";
		else std::cout << "0";
		metameta = metameta << 1ull;
	}
	std::cout << "\n";
}

Game::Game(){
  skippedTurn = false;
  player1 = true;

#ifdef MASKCHECK //use -DMASKCHECK to see some things. they're irrelevant now.
	std::cout << "Testing masks:\nCorner Mask:\t"; printMask(cornerMask);
	std::cout << "edgeMask:\t"; printMask(edgeMask);
#endif

}

#ifndef DEFAULT_DEPTH
#define DEFAULT_DEPTH 7
#endif

int thinkfast = DEFAULT_DEPTH; //How many moves ahead AI thinks

void compareHeuristics(Board desk){ //Never used now. Remnant of debugging
	int vladVal = boardEval(desk);
	//																			x																o
	int sergVal = sergEval(desk.boardPlaced & desk.boardX, desk.boardPlaced - desk.boardX );

	if(vladVal == sergVal) std::cout << "The sergey and the vlad algorithm agree\n";
	else std::cout << "The 2 disagree, Vlad : " << vladVal << " Sergey : " << sergVal << '\n';
}

char getHumanMove(){
	char move;
  int row;
  int col;
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
	return move;
}

double linearCount(Board in){
	double ans = 0.0;
	for(int i = 0; i < 64; i++){
		if(in.boardX & (1ull << i)) ans++;
		else if (in.boardPlaced & (1ull << i)) ans--;
	}
	return ans;
}


std::chrono::duration<double> longest;

bool Game::move(){
  std::cout << field.boardString() << '\n';
  std::vector<char> options = field.getAllowedMoves();
  char move;
  int row;
  int col;
	auto start = std::chrono::steady_clock::now();
  if((options.size() == 0 && skippedTurn)){
    std::cout << "Game Over\nX:" << (int)(field.getXCount()) << " O:"<< (int)(field.getOCount());
		std::cout << "\nLongest AI thought:" << longest.count() << std::endl;
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
	//	compareHeuristics(field); //remnants of ye olde heuristic checks
    if(player1){
			//use -DHUMAN when you want to fight it yourself
			//#ifdef HUMAN
			move = getHumanMove();
			//#else
			//start = std::chrono::steady_clock::now();
			//move = bestMove(field, true, thinkfast, boardEval);
			//#endif
			field.move(move);
      player1 = field.xMove();
    }
    else{ //------------------------------------------Enemy AI time
          //Currently, it's bad
			start = std::chrono::steady_clock::now();
			incidences = 0;
			//move = bestMove(field,false, thinkfast);
														//False as we calculate for O, not X
																									//We can toy about with different heuristic functions
			field.move(bestMove(field,false, thinkfast, boardEval));
			std::cout << "AI moved "; charToString(move);
			player1 = field.xMove();

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
	    std::cout << "elapsed time: " << elapsed_seconds.count() << "s, analysing " << incidences<<" boards to " << thinkfast <<" moves ahead\n";
			if(elapsed_seconds > longest) longest = elapsed_seconds;

#ifdef DYNAMIC_DEPTH //When compiling, use -DDYNAMIC_DEPTH for it to thinkfast
			if(elapsed_seconds.count() < 0.1) thinkfast++;
			else if (elapsed_seconds.count() > 0.25) thinkfast--;
#endif


    }
		//std::cout << "BoardValue = " << boardEval(field) << '\n';
  }
  return true;
}
