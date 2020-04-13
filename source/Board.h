#include<vector>
#include<utility>
#include<string>
#include<cstdint>
class Board{
//Use a char to designate a move

//(char & 240) >> 4 to get row#
//(char & 15) to get col#
unsigned long long int boardPlaced; //64 bits
unsigned long long int boardX;			//64 bits

unsigned char moves = 0;			//8 bits
bool xTurn;										//8 bits

//144 BITS, or 18 BYTES
//OVER 10 FOLD SPACE OPTIMISATION


public:
	Board();
	//Board(const Board& copy);// is not needed as no pointers to outside self
	//~Board();// is not needed as everything fits in one

	unsigned long long int charToMask(char pos) const;

	char move(char pos); //the char return is how many are converted
	char testDir(char mov, char tst) const;
	char changeDir(char pos, char tst);
	void setSquare(char pos, bool isX);
	char allowedMove(char pos) const; //see above
	char getMoves() const;//How many moves are done
	bool xMove() const;
	bool oMove() const;
	std::vector<char> getAllowedMoves(); //can change the turn, hence non-const
	char getSquare(char r, char c) const;
	char getSquare(int r, int c) const;
	char getSquare(char in) const;
	bool getXTurn() const;
	char getXCount() const;
	char getOCount() const;
	char rcToChar(char r, char c) const;
	char rcToChar(int r, int c) const;

	std::string boardString() const;
	void changeSide();
};
