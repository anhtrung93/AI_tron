//#include <iostream>
//#include <fstream>
//#include <cstdlib>
//#include <ai/LowSupport.h>
//#include <ai/DynamicLongest.h>
//#include <ai/Hash.h>
//
//using namespace std;
//
//void unitTest_isSplit(){
//	cout << "Start unitTest_isSplit: \n";
//	const char IS_SPLIT_TEST[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\MyCode\\C++\\AI_Template\\testUnit\\isSplitUnitTest.txt";
//	ifstream is(IS_SPLIT_TEST, ifstream::in);
//
//	int numTest;
//	is >> numTest;
//
//	int * board = new int [MAP_SIZE * MAP_SIZE];
//	Position myPos, enemyPos;
//	for (int idTest = 0; idTest < numTest; ++idTest){
//		for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
//			for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
//				is >> board[CONVERT_COORD(idRow, idCol)];				
//			}
//		}
//		is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y;
//		bool result; int myLongest, enemyLongest;
//		string tmp;
//		is >> tmp >> myLongest >> enemyLongest;
//		if (isSplit(board, myPos, enemyPos) != ((tmp == "true")? true : false)){
//			cout << "Test isSplit #" << idTest << ": " << "failed" << endl;
//		}
//	}
//	delete[] board;
//
//	is.close();
//}
//
//void unitTest_getUpperLongest(){
//	cout << "Start unitTest_getUpperLongest: \n";
//	const char UPPER_LONGEST_TEST[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\MyCode\\C++\\AI_Template\\testUnit\\upperLongestUnitTest.txt";
//	ifstream is(UPPER_LONGEST_TEST, ifstream::in);
//
//	int numTest;
//	is >> numTest;
//
//	int * board = new int[MAP_SIZE * MAP_SIZE];
//	Position myPos, enemyPos;
//	for (int idTest = 0; idTest < numTest; ++idTest){
//		for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
//			for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
//				is >> board[CONVERT_COORD(idRow, idCol)];
//			}
//		}
//		is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y;
//		bool result; int myLongest, enemyLongest;
//		string tmp;
//		is >> tmp >> myLongest >> enemyLongest;
//		if (myLongest != getUpperLongest(board, myPos) 
//				|| enemyLongest != getUpperLongest(board, enemyPos)){
//			cout << "Test isSplit #" << idTest << ": " << "failed" << endl;
//		}
//	}
//	delete[] board;
//
//	is.close();
//}
//
//void testTimeEachFunct(int * board, const Position & myPos, const Position & enemyPos){
//	unsigned long long time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		isSplit(board, myPos, enemyPos);
//	}
//	cout << "Time = " << (getCurTime() - time) << endl;
//	time = getCurTime();
//	for (int count = 0; count < 5000000; ++count){
//		getUpperLongest(board, myPos);
//	}
//	cout << "Time = " << (getCurTime() - time) << endl;
//
//	time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		heurEstLongest(board, myPos);
//	}
//	cout << "Time = " << (getCurTime() - time) << endl;
//
//	time = getCurTime();
//	for (int count = 0; count < 100; ++count){
//		heurEstLongest2(board, myPos);
//	}
//	cout << "Time = " << (getCurTime() - time) << endl;
//
//	time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		heurEstForNonSplit(board, myPos, enemyPos, true);
//	}
//	cout << "Time = " << (getCurTime() - time) << endl;
//}
//
//int main(void){
//	unitTest_isSplit();
//	unitTest_getUpperLongest();
//
//	system("PAUSE");
//	return 0;
//}