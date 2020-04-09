#include"Board.h"
#include<iostream>
Board::Board(){
	for(char a = 0; a < 8; a++){
		for(char b = 0; b < 8; b++){
			board[rcToChar(a,b)] = 0;
		}

	}

	board[0x33] = 3;
	board[0x44] = 3;
	board[0x43] = 1;
	board[0x34] = 1;
	moves = 4;
}
bool Board::xMove() const{
	return !oMove();
}
bool Board::oMove() const{
	return moves % 2;
}
char Board::getOCount() const{
	char ans = 0;
	for(char iter = 0;  iter < 127; iter++)
		if(board[iter] == 1)
			ans++;
	return ans;
}
char Board::getXCount() const{
	return moves - getOCount();
}
char Board::getSquare(char in) const{
	return board[in];
}
char Board::getSquare(char r, char c) const{
	char pnt = c;
	pnt += (r << 4);
	return board[pnt];
}
char Board::getSquare(int r, int c) const{
	char pnt = (c & 0xF);
	pnt += ((r & 0xF) << 4);
	return board[pnt];
}

char Board::allowedMove(char pos) const{
	if(board[pos]) return 0;
	if(getSquare(pos)) return 0;
	char conv = 0;  //guaranteed conversions
	char reach = 0;	//possible conversions
	char combo = 0;	//"locked" conversions
	char ours = oMove()?1:3;
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
	char pC = pos & 0xF;
	char pR = pos >> 4;
	char tR = tst >> 4;
	char tC = tst & 0xF;
	if(!board[tst]) return 0;
	if(tst & 0x88) return 0; // nope if hit on 10001000 mask, good for checking OOB
	if(pos == tst) return 0; //obvs
	if((pC - tC != 1 && tC - pC != 1 && pC != tC) ||
		(pR - tR != 1 && tR - pR != 1 && pR != tR)) return 0; //return 0 if the 2 tested aren't neighbours

	bool 	up = (tR < pR),
				down = (tR > pR),
				left = (tC < pC),
				right = (tC > pC);
	//std::cout << "pR:" << (int)pR << " pC:" << (int)pC << " tC:" << (int)tC << " tR:" << (int)tR << '\n';

	char ans = 0;
	char combo = 0;
	char move = oMove()?1:3;
	//std::cout << "UDLR:" << up << down << left << right <<"\n";
	while((tR < 8 && !(tR & 0x80)) && (tC < 8 && !(tC & 0x80)) && getSquare(tR, tC)){
		if(getSquare(tR, tC) == move){
			ans += combo; combo = 0;
		}
		else
			combo++;
		if(up) tR--;
		if(down) tR++;
		if(left) tC--;
		if(right) tC++;
	}
	//std::cout << "testDir done for "<< ans <<"\n";
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
	toRet += "x|0|1|2|3|4|5|6|7|->C\n--------------------\n";
	for(char a = 0; a < 8; a++){
		toRet += (char)(48+a);
		toRet += "|";
		for(char b = 0; b < 8; b++){
			if(getSquare(a, b)) toRet += (getSquare(a,b)&2)?"X|":"O|";
			else toRet += allowedMove(rcToChar(a,b))?"?|":" |";
		}
		toRet += "\n--------------------\n";
	}
	toRet += "|\nV\nR\n";
	toRet += "Turn of ";
	toRet += xMove()?"X":"O";
	return toRet;
}


std::vector<char> Board::getAllowedMoves() const{
	std::vector<char> v;
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
	if(!board[tst]) return 0;
	if(tst & 0x88) return 0; // nope if hit on 10001000 mask, good for checking OOB
	if(pos == tst) return 0; //obvs
	if((pC - tC != 1 && tC - pC != 1 && pC != tC) ||
		(pR - tR != 1 && tR - pR != 1 && pR != tR)) return 0; //return 0 if the 2 tested aren't neighbours

	bool 	up = (tR < pR),
				down = (tR > pR),
				left = (tC < pC),
				right = (tC > pC);
	//std::cout << "pR:" << (int)pR << " pC:" << (int)pC << " tC:" << (int)tC << " tR:" << (int)tR << '\n';

	char ans = 0;
	char combo = 0;
	char move = oMove()?1:3;
	//std::cout << "UDLR:" << up << down << left << right <<"\n";
	while((tR < 8 && !(tR & 0x80)) && (tC < 8 && !(tC & 0x80)) && getSquare(tR, tC)){
		if(getSquare(tR, tC) == move){
			cR = tR;
			cC = tC;
			for(char d = 1; d <= combo; d++){
				if(up){cR = tR + d;}
				if(down){cR = tR - d;}
				if(left){cC = tC + d;}
				if(right){cC = tC - d;}
				chg = rcToChar(cR, cC);
				board[chg] = move;
			}
		}
		else
			combo++;
		if(up) tR--;
		if(down) tR++;
		if(left) tC--;
		if(right) tC++;
	}
	//std::cout << "testDir done for "<< ans <<"\n";
	return ans;
}
char Board::move(char pos){
	if(!allowedMove(pos)) return 0; //will be optimised out when game class is made
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
	board[pos] = xMove()?3:1;
	moves++;
	return ans;

}
