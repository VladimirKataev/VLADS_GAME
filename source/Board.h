#include<vector>
#include<utility>
#include<string>
class Board{
//Use a char to designate a move

//(char & 240) >> 4 to get row#
//(char & 15) to get col#
unsigned char board[256];
/*a char is
	0 for uninhabited, 	so we can if(board[pos]) for taken
	1 for O,						so we can &2 to get isBlack(pos)
	3 for X							so we can &2
*/
unsigned char moves = 0;			//we only need 64, moves%2 returns whiteMove


//257 bytes of internal data, no pointers to oustide


public:
	Board();
	//Board(const Board& copy);// is not needed as no pointers to outside self
	//~Board();// is not needed as everything fits in one


	char move(char pos); //the char return is how many are converted
	char testDir(char mov, char tst) const;
	char changeDir(char pos, char tst);

	char allowedMove(char pos) const; //see above
	char getMoves() const;//How many moves are done
	bool xMove() const;
	bool oMove() const;
	std::vector<char> getAllowedMoves() const;
	char getSquare(char r, char c) const;
	char getSquare(int r, int c) const;
	char getSquare(char in) const;

	char getXCount() const;
	char getOCount() const;
	char rcToChar(char r, char c) const;
	char rcToChar(int r, int c) const;

	std::string boardString() const;

};
