#include<iostream>
#include<algorithm>
#include"Board.h"
using namespace std;


void charToString(char in){
	cout << (char)(48 + (in >> 4)) << "," << (char)(48 + (in & 7)) << " ";
}

int main(){
	cout << "making Board obj\n";
	Board tst;
	vector<char> moves;

	cout << "made it\n";
	cout << tst.boardString() << endl;
	cout << "got here\n";
	cout << "Allowed move at 5,3 = " <<(int)( tst.allowedMove(0x53)) << endl;
	cout << "Allowed move at 4,4 = " << (int)(tst.allowedMove(0x44)) << endl;
	cout << "O count is at " << tst.getOCount() << endl;
	cout << "X count is at " << tst.getXCount() << endl;

	return 0;
}
