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
	char conv = 0;  //guaranteed conversions
	char reach = 0;	//possible conversions
	char combo = 0;	//"locked" conversions
	char ours = oMove()?1:3;
	//std::cout << "move is " << ours << std::endl;
	char r = pos >> 4;
	char c = pos & 0xF;
	//std::cout << "R=" << r << " and C=" << c << std::endl;
	//conv = ((r << 4) + c); remnants of debugging
	if(board[pos]) return 0;
	bool broke = false;

	reach = 0; broke = false;
	for(char u = r + 1, v = c + 1; u >= 0 && u < 8 && v >= 0 && v < 8 && !getSquare(u,v); u++, v++){ // down-right
		if(u == r + 1 && v == c + 1 && getSquare(u,v) = ours)
			broke = true;
		else{
			if(getSquare(u,v) == ours){
				reach++;
			}
			else{
					conv += reach;
					reach = 0;
			}
		}
	}
	reach = 0; broke = false;
	for(char u = r - 1, v = c + 1; u >= 0 && u < 8 && v >= 0 && v < 8 && !getSquare(u,v); u--, v++){ // up-right
		if(u == r - 1 && v == c + 1 && getSquare(u,v) = ours)
			broke = true;
		else{
			if(getSquare(u,v) == ours){
				reach++;
			}
			else{
					conv += reach;
					reach = 0;
			}
		}
	}
	reach = 0; broke = false;
	for(char u = r - 1, v = c - 1; u >= 0 && u < 8 && v >= 0 && v < 8 && !getSquare(u,v); u--, v--){ // down-left
		if(u == r - 1 && v == c - 1 && getSquare(u,v) = ours)
			broke = true;
		else{
			if(getSquare(u,v) == ours){
				reach++;
			}
			else{
					conv += reach;
					reach = 0;
			}
		}
	}
	reach = 0; broke = false;
	for(char u = r + 1, v = c - 1; u >= 0 && u < 8 && v >= 0 && v < 8 && !getSquare(u,v); u++, v--){ // up-left
		if(u == r + 1 && v == c - 1 && getSquare(u,v) = ours)
			broke = true;
		else{
			if(getSquare(u,v) == ours){
				reach++;
			}
			else{
					conv += reach;
					reach = 0;
			}
		}
	}




	return conv;
}

char Board::rcToChar(char r, char c) const{
	return (char)(c + (r << 4));
}
char Board::rcToChar(int r, int c) const{
	return (char)(c + (r << 4));
}

std::string Board::boardString() const{
	std::string toRet;
	toRet += "x|01234567\n-----------\n";
	for(char a = 0; a < 8; a++){
		toRet += (char)(48+a);
		toRet += "|";
		for(char b = 0; b < 8; b++){
			if(getSquare(a, b)) toRet += (getSquare(a,b)&2)?"X":"O";
			else toRet += " ";
		}
		toRet += "\n";
	}
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
