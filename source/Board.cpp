#include"Board.h"
#include<iostream>

char Board::getMoves() const {return moves;}

void printMask(unsigned long long int in){
	unsigned long long int meta = (1ull << 63);
	unsigned long long int metameta = 1;
	std::cout << "Mask :";
	while(meta != metameta){
		if(in & metameta) std::cout << "1";
		else std::cout << "0";
		metameta = metameta << 1ull;
	}
	std::cout << "\n";
}

void Board::setSquare(char pos, bool isX){
	unsigned long long int p = charToMask(pos);
	//std::cout << "Changed R" << (pos >> 4) << " C" << (pos & 0xf) << '\n';
	boardPlaced |= p;
	if(isX)
		boardX |= p;
	else
		boardX &= ~p;
}

unsigned long long int Board::charToMask(char pos) const{
	//note:
	//DOES NOT WORK IF HIT ON POS & 0X88
	//pos = ((pos & 0x7) + ((pos & 0x70) >> 1) & 0x3f);
	char rp = (pos & 0xf0) >> 1;
	unsigned long long int p = (1ull << (rp + (pos & 7ull)));
	//printMask(p);
	return p;
				//1 bit moved left by the rows moved left + col
}


Board::Board(){
	boardPlaced = 0;
	boardX = 0;
	moves = 4;
	xTurn = true;

	setSquare(0x33, true);
	setSquare(0x44, true);
	setSquare(0x34, false);
	setSquare(0x43, false);



}
bool Board::xMove() const{
	return xTurn;
}
bool Board::oMove() const{
	return !xTurn;
}
char Board::getOCount() const{
	char ans = 0;
	for(char r = 0; r < 8; r++){
		for(char c = 0; c < 8; c++){
			if(getSquare(r,c) == 1) ans++;
		}
	}
	return ans;
}
char Board::getXCount() const{
	char ans = 0;
	for(char r = 0; r < 8; r++){
		for(char c = 0; c < 8; c++){
			if(getSquare(r,c) == 3) ans++;
		}
	}
	return ans;
}
char Board::getSquare(char in) const{ //Return 0 for empty, 1 for O, 3 for X //TESTED, WORKS
		unsigned long long int mask = charToMask(in);
		if(mask & boardX) return 3;
		if(mask & boardPlaced) return 1;
		return 0;
}
char Board::getSquare(char r, char c) const{ //Return 0 for empty, 1 for O, 3 for X //TESTED, WORKS
	char pnt = c;
	pnt += (r << 4);
	return getSquare(pnt);
}
char Board::getSquare(int r, int c) const{ //Return 0 for empty, 1 for O, 3 for X
	char pnt = (c & 0xF);
	pnt += ((r & 0xF) << 4);
	return getSquare(pnt);
}

char Board::allowedMove(char pos) const{
	if(boardPlaced & charToMask(pos)) return 0; //not allowed to move into a taken spot

	char row = pos >> 4;
	char col = pos & 0xF;
	return
	testDir(pos, rcToChar(row+1,col))+
	testDir(pos, rcToChar(row-1,col))+
	testDir(pos, rcToChar(row+1,col+1))+
	testDir(pos, rcToChar(row+1,col-1))+
	testDir(pos, rcToChar(row-1,col+1))+
	testDir(pos, rcToChar(row-1,col-1))+
	testDir(pos, rcToChar(row,col-1))+
	testDir(pos, rcToChar(row,col+1));
}

char Board::testDir(char pos, char tst) const{
	//std::cout << "Coords " << (int) pos << " Dir " << (int) tst << '\n';
	char pC = pos & 0x7;
	char pR = pos >> 4;
	char tR = tst >> 4;
	char tC = tst & 0x7;
	bool 	up = (tR < pR),
				down = (tR > pR),
				left = (tC < pC),
				right = (tC > pC);

	char move = xTurn?3:1;
	if(tst & 0x88) return 0;
	if(!getSquare(tst) || getSquare(tst) == move) return 0;
	if(pos == tst) return 0;
	if(pC - tC != 1 && pC != tC && tC - pC != 1) return 0;
	if(pR - tR != 1 && pR != tR && tR - pR != 1) return 0;
	if(getSquare(pos)) return 0;
	char ans = 0;
	char combo = 0;

	while((tR < 8 && !(tR & 0x80)) && (tC < 8 && !(tC & 0x80)) && getSquare(tR, tC)){
		if(getSquare(tR, tC) == move){
			ans += combo; combo = 0;
			return ans;
		}
		else
			combo++;
		if(up) tR--;
		if(down) tR++;
		if(left) tC--;
		if(right) tC++;
	}


	/*
	if(!getSquare(pR, pC)) return 0;
	if(tst & 0x88) return 0; // nope if hit on 10001000 mask, good for checking OOB
	if(pos == tst) return 0; //obvs
	if((pC - tC != 1 && tC - pC != 1 && pC != tC) ||
		(pR - tR != 1 && tR - pR != 1 && pR != tR)) return 0; //return 0 if the 2 tested aren't neighbours

	//std::cout << "pR:" << (int)pR << " pC:" << (int)pC << " tC:" << (int)tC << " tR:" << (int)tR << '\n';

	char ans = 0;
	char combo = 0;
	char move = xTurn?3:1;
	if(getSquare(tR, tC) == move) return 0;

	//std::cout << "UDLR:" << up << down << left << right <<"\n";
	while((tR < 8 && !(tR & 0x80)) && (tC < 8 && !(tC & 0x80)) && getSquare(tR, tC)){
		if(getSquare(tR, tC) == move){
			ans += combo; combo = 0;
			return ans;
		}
		else
			combo++;
		if(up) tR--;
		if(down) tR++;
		if(left) tC--;
		if(right) tC++;
	}
	//std::cout << "testDir done for "<< ans <<"\n";
	*/
	return ans;
}

char Board::rcToChar(char r, char c) const{
	return (char)(c + (r << 4));
}
char Board::rcToChar(int r, int c) const{
	return (char)(c + (r << 4));
}

std::string Board::boardString() const{
	std::string toRet;
	toRet += "x|0|1|2|3|4|5|6|7|->C\n------------------\n";
	for(char a = 0; a < 8; a++){
		toRet += (char)(48+a);
		toRet += "|";
		for(char b = 0; b < 8; b++){
			if(getSquare(a, b)) toRet += (getSquare(a,b)&2)?"X|":"O|";
			else toRet += allowedMove(rcToChar(a,b))?"?|":" |";
		}
		toRet += "\n------------------\n";
	}
	toRet += "|\nV\nR-----------------\n";
	toRet += "Turn of ";
	toRet += xTurn?"X\n":"O\n";
	/*
	toRet += "Board Mask ";
	printMask(boardPlaced);
	toRet += "X mask ";
	printMask(boardX);
	*/
	return toRet;
}


std::vector<char> Board::getAllowedMoves(){
	std::vector<char> v;
	v.reserve(32);
	for(char a = 0; a < 8; a++){
		for(char b = 0; b < 8; b++){
			if(allowedMove((a << 4) + b))
				v.push_back((a << 4) + b);
		}
	}
	return v;
}

char Board::changeDir(char pos, char tst){
	char pC = pos & 0xF;
	char pR = pos >> 4;
	char tR = tst >> 4;
	char tC = tst & 0xF;
	char cR, cC, chg;
	bool 	up = (tR < pR),
				down = (tR > pR),
				left = (tC < pC),
				right = (tC > pC);

	char combo = testDir(pos, tst);
	cC = pC;
	cR = pR;
	for(int d = 0; d < combo; d++){
		if(up) cR--;
		if(down) cR++;
		if(left) cC--;
		if(right) cC ++;
		setSquare(rcToChar(cR, cC), xTurn);
	}


	return combo;
}
char Board::move(char pos){
	//if(!allowedMove(pos)) return 0; //will be optimised out when game class is made
	char r = pos >> 4;
	char c = pos & 0xF;
	char ans =
	changeDir(pos, rcToChar(r + 1, c))+
	changeDir(pos, rcToChar(r - 1, c))+
	changeDir(pos, rcToChar(r + 1, c+1))+
	changeDir(pos, rcToChar(r + 1, c-1))+
	changeDir(pos, rcToChar(r - 1, c+1))+
	changeDir(pos, rcToChar(r - 1, c-1))+
	changeDir(pos, rcToChar(r, c - 1))+
	changeDir(pos, rcToChar(r, c+1));
	if(ans){
		setSquare(pos, xTurn);
		moves++;
		xTurn = !xTurn;
	}
	return ans;

}

void Board::changeSide(){
	xTurn = !xTurn;
}
bool Board::getXTurn() const{
	return xTurn;
}
