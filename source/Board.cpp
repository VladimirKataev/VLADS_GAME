#include"Board.h"

Board::Board(){
	for(int a = 0; a < 64; a++) *(area + a) = 0;
	*(area + (24 + 3)) = 1;
	*(area + (24 + 4)) = 3;
	*(area + (32 + 3)) = 3;
	*(area + (32 + 4)) = 1;
	whiteTurn = true;
};
Board::Board(const Board& copy){
	for(int a = 0; a < 8; a++){
		for(int b = 0; b < 8; b++){
			*(area + (a*8 + b)) =
			copy.getSquare(std::pair<int, int>(a, b));
		}
	}
	whiteTurn = copy.getWhiteTurn();
}
Board::~Board(){
	//Board object itself never allocated memory external to itself
};

char Board::getSquare(std::pair<int, int> pos) const{
	return *(area + ((pos.first * 8)+ pos.second));
}
bool Board::getWhiteTurn() const{
	return whiteTurn;
}
std::string Board::boardString() const{
	std::string toRet;
	toRet += "x|01234567\n-----------\n";
	for(int a = 0; a < 64; a++){
		if((a % 8 == 0)){
			if(a)
				toRet += "|\n";
			toRet += (char)(48+(a/8));
			toRet += "|";
		}

		if(*(area + a) & 1)
			toRet += *(area + a) + 48; //0 for white, 1 for black
		else
			toRet += " ";
	}
	toRet += "|";
	return toRet;
}
int Board::getWhiteCount() const{
	int ans = 0;
	for(int a = 0; a < 64; a++){
		if(*(area + a) == 1)
			ans++;
	}
	return ans;
}
int Board::getBlackCount() const{
	int ans = 0;
	for(int a = 0; a < 64; a++){
		if(*(area + a) == 3)
			ans++;
	}
	return ans;
}
std::vector<std::pair<int, int>> Board::getMoves() const{
	std::vector<std::pair<int, int>> toRet;
	for(int r = 0; r < 8; r++){
		for(int c = 0; c < 8; c++){
			if(isAllowedMove(std::pair<int, int>(r, c)))
				toRet.push_back(std::pair<int, int> (r, c));
		}
	}
}
int Board::isAllowedMove(std::pair<int, int> in) const{
	return 0;

}
