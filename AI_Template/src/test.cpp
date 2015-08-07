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
//	int * board = new int[MAP_SIZE * MAP_SIZE];
//	Position myPos, enemyPos;
//	for (int idTest = 0; idTest < numTest; ++idTest){
//		for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
//			for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
//				is >> board[CONVERT_COORD(idRow, idCol)];
//			}
//		}
//		is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y;
//		int myLongest, enemyLongest;
//		string tmp;
//		is >> tmp >> myLongest >> enemyLongest;
//		for (int id = 0; id < 2; ++id){
//			if (isSplit(board, myPos, enemyPos) != ((tmp == "true") ? true : false)){
//				cout << "Test isSplit #" << idTest << ": " << "failed" << endl;
//				if (id == 0){
//					++id;
//					continue;
//				}
//			}
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
//		int myLongest, enemyLongest;
//		string tmp;
//		is >> tmp >> myLongest >> enemyLongest;
//		for (int id = 0; id < 2; ++id){
//			if (myLongest != getUpperLongest(board, myPos) || enemyLongest != getUpperLongest(board, enemyPos)){
//				cout << "Test getUpperLongest #" << idTest << ": " << "failed" << endl;
//				if (id == 0){
//					++id;
//					continue;
//				}
//			}
//		}
//
//	}
//	delete[] board;
//	is.close();
//}
//
//void unitTest_heurEstLongest(){
//	cout << "Start unitTest_heurEstLongest: \n";
//	const char UPPER_LONGEST_TEST[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\MyCode\\C++\\AI_Template\\testUnit\\heurEstLongestUnitTest.txt";
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
//		int depth;
//		is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y >> depth;
//		int heurEstLongestResult1, heurEstLongestResult2;
//		is >> heurEstLongestResult1 >> heurEstLongestResult2;
//		int heurEstResult, minimaxValue;
//		is >> heurEstResult >> minimaxValue;
//		for (int id = 0; id < 2; ++id){
//			int heurEstLongest1 = heurEstLongest(board, myPos);
//			int heurEstLongestSecond = heurEstLongest2(board, myPos);
//			if (heurEstLongest1 != heurEstLongestResult1 || heurEstLongestSecond != heurEstLongestResult2){
//				cout << "Test getUpperLongest #" << idTest << ": " << "failed"
//					<< heurEstLongest1 << " " << heurEstLongestResult1 << " " << heurEstLongestSecond << " " << heurEstLongestResult2
//					<< " " << getUpperLongest(board, myPos) << endl;
//				if (id == 0){
//					++id;
//					continue;
//				}
//			}
//		}
//
//	}
//	delete[] board;
//	is.close();
//}
//
//void unitTest_heurEst(){
//	cout << "Start unitTest_heurEst: \n";
//	const char IS_SPLIT_TEST[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\MyCode\\C++\\AI_Template\\testUnit\\heurEstUnitTest.txt";
//	ifstream is(IS_SPLIT_TEST, ifstream::in);
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
//		int depth;
//		is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y >> depth;
//		int heurEstLongestResult1, heurEstLongestResult2;
//		is >> heurEstLongestResult1 >> heurEstLongestResult2;
//		int heurEstResult, minimaxValue;
//		is >> heurEstResult >> minimaxValue;
//		bool isSplitStatus = isSplit(board, myPos, enemyPos);
//		for (int id = 0; id < 2; ++id){
//			if (isSplitStatus){
//				int heurEst = heurEstForSplit(board, myPos, enemyPos, true);
//				if (heurEst != heurEstResult){
//					cout << "Test heurEst #" << idTest << ": " << "failed " << heurEst << " " << heurEstResult << endl;
//					if (id == 0){
//						++id;
//						continue;
//					}
//				}
//			}
//			else {
//				int heurEst = heurEstForNonSplit(board, myPos, enemyPos, true);
//				if (heurEst != heurEstResult){
//					cout << "Test heurEst #" << idTest << ": " << "failed " << heurEst << " " << heurEstResult << endl;
//					if (id == 0){
//						++id;
//						continue;
//					}
//				}
//			}
//		}
//	}
//	delete[] board;
//
//	is.close();
//}
//
//void unitTest_minimax(){
//	cout << "Start unitTest_minimax: \n";
//
//	const char MINIMAX_TEST[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\MyCode\\C++\\AI_Template\\testUnit\\minimaxUnitTest.txt";
//	ifstream is(MINIMAX_TEST, ifstream::in);
//
//	int numTest;
//	is >> numTest;
//
//	int * board = new int[MAP_SIZE * MAP_SIZE];
//	Position myPos, enemyPos;
//	for (int idTest = 0; idTest < numTest; ++idTest){
//		initZobrist();
//		for (int idDepth = 0; idDepth < 20; ++idDepth){
//			removeDatabaseAtDepth(idDepth);
//		}
//		for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
//			for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
//				is >> board[CONVERT_COORD(idRow, idCol)];
//			}
//		}
//		int depth;
//		is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y >> depth;
//		int heurEstLongestResult1, heurEstLongestResult2;
//		is >> heurEstLongestResult1 >> heurEstLongestResult2;
//		int heurEstResult, minimaxValue;
//		is >> heurEstResult >> minimaxValue;
//		//cout << "test " << idTest << ": " << depth << endl;
//		unsigned long long hashVal = hashBoard(board, myPos, enemyPos, true);
//		StateInfo * info = getStateInfo(0, hashVal);
//		int calVal = 0;
//		for (int idDepth = 1; idDepth <= depth; ++idDepth){
//			//calVal = minimax(board, myPos, enemyPos, 0, idDepth, -INF, INF, hashVal, info, true);
//			//calVal = mtdf(board, myPos, enemyPos, calVal, idDepth, hashVal, info);
//			calVal = minimax_scout(board, myPos, enemyPos, 0, idDepth, -INF, INF, hashVal, info, true);
//		}		
//		if ((abs(minimaxValue) < 500 && (minimaxValue != calVal)) || (abs(minimaxValue) >= 300 && minimaxValue * calVal < 0)){
//			cout << "Test minimax #" << idTest << ": " << "failed " << minimaxValue << " " << calVal << endl;
//		}
//	}
//	delete[] board;
//	is.close();
//}
//
//void testTimeEachFunct(){
//	const char UPPER_LONGEST_TEST[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\MyCode\\C++\\AI_Template\\testUnit\\isSplitUnitTest.txt";
//	ifstream is(UPPER_LONGEST_TEST, ifstream::in);
//
//	int numTest;
//	is >> numTest;
//
//	int * board = new int[MAP_SIZE * MAP_SIZE];
//	Position myPos, enemyPos;
//	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
//		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
//			is >> board[CONVERT_COORD(idRow, idCol)];
//		}
//	}
//	is >> myPos.x >> myPos.y >> enemyPos.x >> enemyPos.y;
//
//	unsigned long long time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		isSplit_A_star(board, myPos, enemyPos);
//	}
//	cout << "Time isSplit_A_star = " << (getCurTime() - time) << endl;
//	time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		isSplit(board, myPos, enemyPos);
//	}
//	cout << "Time isSplit = " << (getCurTime() - time) << endl;
//	time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		isSplit_bfs(board, myPos, enemyPos);
//	}
//	cout << "Time isSplit_bfs = " << (getCurTime() - time) << endl;
//	time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		getUpperLongest(board, myPos);
//	}
//	cout << "Time getUpperLongest = " << (getCurTime() - time) << endl;
//	time = getCurTime();
//	for (int count = 0; count < 1000; ++count){
//		getUpperLongest_old(board, myPos);
//	}
//	cout << "Time getUpperLongest_old = " << (getCurTime() - time) << endl;
//
//	time = getCurTime();
//	for (int count = 0; count < 1; ++count){
//		heurEstLongest(board, myPos);
//	}
//	cout << "Time heurEstLongest = " << (getCurTime() - time) << endl;
//
//	time = getCurTime();
//	for (int count = 0; count < 1; ++count){
//		heurEstLongest2(board, myPos);
//	}
//	cout << "Time heurEstLongest2 = " << (getCurTime() - time) << endl;
//
//	time = getCurTime();
//	for (int count = 0; count < 1; ++count){
//		heurEstForNonSplit(board, myPos, enemyPos, true);
//	}
//	cout << "Time heurEstForNonSplit = " << (getCurTime() - time) << endl;
//
//	/*time = getCurTime();
//	for (int depth = 1; depth < 12; ++depth){
//		minimax_old(board, myPos, enemyPos, 0, 12, -INF, INF);
//	}
//	cout << "Time minimax_old = " << (getCurTime() - time) << endl;*/
//
//	initZobrist();
//	for (int idDepth = 0; idDepth < 25; ++idDepth){
//		removeDatabaseAtDepth(idDepth);
//	}
//	unsigned long long hashVal = hashBoard(board, myPos, enemyPos, true);
//	StateInfo * info = getStateInfo(0, hashVal);
//	minimax_scout(board, myPos, enemyPos, 0, 16, -INF, INF, hashVal, info);
//	time = getCurTime();
//	for (int depth = 12; depth < 19; ++depth){
//		unsigned long long time2 = getCurTime();
//		int result = minimax_scout(board, myPos, enemyPos, 0, depth, -INF, INF, hashVal, info);
//		cout << "Time depth " << depth << " minimax_scout = " << (getCurTime() - time2) << " " << result << endl;
//	}
//	cout << "Time minimax_scout = " << (getCurTime() - time) << endl;
//
//	totalDepth = 0;
//	initZobrist();
//	for (int idDepth = 0; idDepth < 25; ++idDepth){
//		removeDatabaseAtDepth(idDepth);
//	}	
//	hashVal = hashBoard(board, myPos, enemyPos, true);
//	info = getStateInfo(0, hashVal);
//	mtdf(board, myPos, enemyPos, 0, 16, hashVal, info);
//	time = getCurTime();
//	int val = 0;
//	for (int depth = 12; depth < 19; ++depth){
//		unsigned long long time2 = getCurTime();
//		val = mtdf(board, myPos, enemyPos, val, depth, hashVal, info);
//		cout << "Time depth " << depth << " mtdf = " << (getCurTime() - time2) << " " << val << endl;
//	}
//	cout << "Time mtdf = " << (getCurTime() - time) << endl;
//
//
//	initZobrist();
//	for (int idDepth = 0; idDepth < 25; ++idDepth){
//		removeDatabaseAtDepth(idDepth);
//	}
//	hashVal = hashBoard(board, myPos, enemyPos, true);
//	info = getStateInfo(0, hashVal);
//	minimax(board, myPos, enemyPos, 0, 16, -INF, INF, hashVal, info);
//	time = getCurTime();
//	for (int depth = 12; depth < 19; ++depth){
//		unsigned long long time2 = getCurTime();
//		int result = minimax(board, myPos, enemyPos, 0, depth, -INF, INF, hashVal, info);
//		cout << "Time depth " << depth << " minimax = " << (getCurTime() - time2) << " " << result << endl;
//	}
//	cout << "Time minimax = " << (getCurTime() - time) << endl;
//	
//	delete[] board;
//}
//
//int main(void){
//	unitTest_isSplit();
//	unitTest_getUpperLongest();
//	unitTest_heurEstLongest();
//	unitTest_heurEst();
//	unitTest_minimax();
//	testTimeEachFunct();
//	return 0;
//}