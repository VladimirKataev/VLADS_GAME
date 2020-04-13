#include<iostream>
#include<algorithm>
//#include"Board.h"
#include"Game.h"
using namespace std;




int main(){

	/*
	Board tst;
	vector<char> moves;

	cout << tst.boardString() << endl;
	cout << "O count is at " << tst.getOCount() << endl;
	cout << "X count is at " << tst.getXCount() << endl;
	moves = tst.getAllowedMoves();
	cout << "Available moves are:";
	for_each(begin(moves), end(moves), charToString);
	cout << "Move at 2,4"<<endl;
	tst.move(tst.rcToChar(2,4));
	cout << tst.boardString() << endl;
	*/

	


	Game run;
	while(run.move()){
		cout << endl;
	}

	return 0;
}
