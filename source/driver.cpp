#include<iostream>
#include<algorithm>
#include"Board.h"
using namespace std;


void charToString(char in){
	cout << "{"<< (int)(in >> 4) << ',' << (int) (in & 0xF) << "} ";
}

int main(){
	Board tst;
	vector<char> moves;

	cout << tst.boardString() << endl;
	cout << "O count is at " << tst.getOCount() << endl;
	cout << "X count is at " << tst.getXCount() << endl;
	moves = tst.getAllowedMoves();
	cout << "Available moves are:";
	for_each(begin(moves), end(moves), charToString);
	cout << endl;



	return 0;
}
