#include<vector>
#include<utility>
#include<string>
class Board{
	unsigned char area[64]; //0 for untaken, 1 for white, 3 for black
	bool whiteTurn;
public:
	Board();// sets the area to be all empty, bar starting grid, black starts
	Board(const Board& copy);
	~Board();


	//note pair<int, int> is equivalent to a move/board position
	char getSquare(std::pair<int, int> pos) const; // rows/cols start at 0
	std::vector<std::pair<int, int>> getMoves() const;
	bool getWhiteTurn() const;
	std::string boardString() const;
	int getWhiteCount() const;
	int getBlackCount() const;
};
