//#include <iostream>
//#include <cstdlib>
//#include <ai/LowSupport.h>
//#include <ai/DynamicLongest.h>
//#include <ai/Hash.h>
//#include <Windows.h>
//
//using namespace std;
//
//unsigned long long getCurTime(){
//	FILETIME ft;
//	LARGE_INTEGER li;
//
//	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
//	* to a LARGE_INTEGER structure. */
//	GetSystemTimeAsFileTime(&ft);
//	li.LowPart = ft.dwLowDateTime;
//	li.HighPart = ft.dwHighDateTime;
//
//	unsigned long long ret = li.QuadPart;
//	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
//	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */
//
//	return ret;
//}
//
//bool isSplitStatus = false;
//int cmd = UNKNOWN_DIRECTION;
//int lastDepthLimitLvl = 0;
//bool winForSure = false;
//
//int getBestMoveInDepth(int * board, const Position & myPos, const Position & enemyPos, int depthLimitLvl){
//	if (isSplitStatus || isSplit(board, myPos, enemyPos)){
//		if (isSplitStatus == false){
//			isSplitStatus = true;
//			depthLimitLvl = START_DEPTH_LEVEL;
//		}
//		int myUpper = getUpperLongest(board, myPos);
//		int enemyUpper = getUpperLongest(board, enemyPos);
//		cout << "myUpper = " << myUpper << " enemyUpper = " << enemyUpper << endl;
//		cout << "heurEstLongest = " << heurEstLongest(board, myPos) << " heurEstLongest2 = " << heurEstLongest2(board, myPos) << endl;
//		return dlsEstLongest(board, myPos, 0, depthLimitLvl, INF);
//	}
//	else {
//		return minimax(board, myPos, enemyPos, 0, depthLimitLvl, -INF, INF);
//	}
//}
//
//
//void normalTest(int * board, const Position & myPos, const Position & enemyPos){
//	cout << "myPos = " << myPos.x << " " << myPos.y << endl;
//	cout << "enemyPos = " << enemyPos.x << " " << enemyPos.y << endl;
//
//	cout << "isSplit = " << ((isSplit(board, myPos, enemyPos) == true) ? "true" : "false") << endl;
//	cout << "myUpperLongest = " << getUpperLongest(board, myPos) << endl;
//	cout << "enemyUpperLongest = " << getUpperLongest(board, enemyPos) << endl;
//
//	cout << "my heurEstLongest = " << heurEstLongest(board, myPos) << endl;
//	cout << "enemy heurEstLongest = " << heurEstLongest(board, enemyPos) << endl;
//
//	cout << "my heurEstLongest2 = " << heurEstLongest2(board, myPos) << endl;
//	cout << "enemy heurEstLongest2 = " << heurEstLongest2(board, enemyPos) << endl;
//
//	//cout << "my calMinDistToAll = ");
//	//print(calMinDistToAll(board, myPos));
//	//cout << "enemy calMinDistToAll = ");
//	//print(calMinDistToAll(board, enemyPos));
//
//	if (isSplit(board, myPos, enemyPos) == false){
//		cout << "heurEstForNonSplit = " << heurEstForNonSplit(board, myPos, enemyPos, true) << endl;
//		cout << " minimax = " << minimax(board, myPos, enemyPos, 0, START_DEPTH_LEVEL, -INF, INF) << endl;
//	}
//	else {
//		cout << "heurEstForSplit = " << heurEstForSplit(board, myPos, enemyPos, 3, START_DEPTH_LEVEL) << endl;
//		cout << "my dlsEstLongest = " << dlsEstLongest(board, myPos, 0, START_DEPTH_LEVEL, getUpperLongest(board, myPos)) << endl;
//		cout << "enemy dlsEstLongest = " << dlsEstLongest(board, enemyPos, 0, START_DEPTH_LEVEL, getUpperLongest(board, enemyPos)) << endl;
//	}
//}
//
//void botRealFightTest(int * board, const Position & myPos, const Position & enemyPos){
//	unsigned long long  startTime = getCurTime();
//
//	//Just a silly bot with random moves
//	vector<int> freeMoves;
//	if (myPos.x > 0 && board[CONVERT_COORD(myPos.x - 1, myPos.y)] == BLOCK_EMPTY)
//	{
//		freeMoves.push_back(DIRECTION_LEFT);
//	}
//	if (myPos.x < MAP_SIZE - 1 && board[CONVERT_COORD(myPos.x + 1, myPos.y)] == BLOCK_EMPTY)
//	{
//		freeMoves.push_back(DIRECTION_RIGHT);
//	}
//	if (myPos.y > 0 && board[CONVERT_COORD(myPos.x, myPos.y - 1)] == BLOCK_EMPTY)
//	{
//		freeMoves.push_back(DIRECTION_UP);
//	}
//	if (myPos.y < MAP_SIZE - 1 && board[CONVERT_COORD(myPos.x, myPos.y + 1)] == BLOCK_EMPTY)
//	{
//		freeMoves.push_back(DIRECTION_DOWN);
//	}
//
//	int size = freeMoves.size();
//
//	cmd = freeMoves[0];
//
//	if (size != 1){
//		int depthLimitLvl = MAX(START_DEPTH_LEVEL, (isSplitStatus) ? (lastDepthLimitLvl - 1) : (lastDepthLimitLvl - 2));
//		winForSure = false;
//		unsigned long long lastTime, newTime = getCurTime();
//		do {
//			lastTime = newTime;
//			cmd = getBestMoveInDepth(board, myPos, enemyPos, depthLimitLvl);
//			++depthLimitLvl;
//			newTime = getCurTime();
//			cout << "Depth: " << (depthLimitLvl - 1) << " time: " << (newTime - lastTime) << endl;
//		} while (startTime + MAX_TIME - newTime > 3 * (newTime - lastTime) && winForSure == false);
//		lastDepthLimitLvl = depthLimitLvl - 1;
//		cout << "Final depth: " << lastDepthLimitLvl << endl;
//		cout << "Time: " << (getCurTime() - startTime) << "ms" << endl;
//		//cmd = getBestMoveInDepth(board, myPos, enemyPos, 12);
//		//printBoard(board);
//		cout << "command = " << cmd << endl;
//	}
//	else {
//		lastDepthLimitLvl -= 2;
//	}
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
//	char FILE_PATH[] = "E:\\Dropbox\\Projects\\GAMELOFT_AI_2015\\test.inp";
//
//	FILE * tmp = freopen(FILE_PATH, "r", stdin);
//
//	int * board = new int[MAP_SIZE * MAP_SIZE];
//	Position myPos, enemyPos;
//
//	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
//		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
//			cin >> board[CONVERT_COORD(idRow, idCol)];
//			if (board[CONVERT_COORD(idRow, idCol)] == BLOCK_PLAYER_1){
//				myPos = Position(idRow, idCol);
//			}
//			else if (board[CONVERT_COORD(idRow, idCol)] == BLOCK_PLAYER_2){
//				enemyPos = Position(idRow, idCol);
//			}
//		}
//	}
//
//	initZobrist();
//	unsigned long long tmp1 = hashBoard(board, myPos, enemyPos, true);
//	unsigned long long tmp2, tmp3;
//	for (int direction = 1; direction <= 4; ++direction){
//		Position newPos = moveDirection(myPos, direction);
//		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
//			tmp2 = hashMove(tmp1, myPos, newPos, true);
//			board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(myPos.x, myPos.y)];
//			++board[CONVERT_COORD(myPos.x, myPos.y)];
//			tmp3 = hashBoard(board, newPos, enemyPos, false);
//			if (tmp2 != tmp3){
//				cout << "Hash Error" << endl;
//			}
//			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;
//			--board[CONVERT_COORD(myPos.x, myPos.y)];
//		}
//	}
//
//	//botRealFightTest(board, myPos, enemyPos);
//	//testTimeEachFunct(board, myPos, enemyPos);
//
//	system("PAUSE");
//	return 0;
//}