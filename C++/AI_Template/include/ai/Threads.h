#pragma once

#include <time.h>
#include <thread>
#include <mutex>
#include <ai/DynamicLongest.h>

#ifdef PRINT_TEST
#include <fstream>
#endif

extern bool timeOut;
extern int totalDepth;
extern bool isSplitStatus;
extern mutex mtx;

void sleepThreadJob(int depth){
	this_thread::sleep_for(chrono::milliseconds(MAX_TIME));
	mtx.lock();
	if (depth == totalDepth){
		timeOut = true;
	}
	mtx.unlock();
}

void stealThreadJob(int * board, Position myPos, Position enemyPos, int curTotalDepth){
#ifdef PRINT_LOG
	unsigned long long startDelTime = getCurTime();
#endif
	removeDatabaseAtDepth(curTotalDepth - 2);
	removeDatabaseAtDepth(curTotalDepth - 1);
#ifdef PRINT_LOG
	cout << "Delete time = " << (getCurTime() - startDelTime) << "ms" << endl;
#endif

	if (isSplitStatus == true || totalDepth != curTotalDepth
#ifdef NOT_STEAL
		|| true
#endif
		){
		if (board != NULL){
			delete[] board;
			board = NULL;
		}
		return;
	}
	
#ifdef PRINT_LOG
	cout << "Start steal depth at " << curTotalDepth << endl;
#endif

	int depthLimitLvl = START_DEPTH_LEVEL;
	int myUpper = getUpperLongest(board, myPos);

	unsigned long long hashVal = hashBoard(board, myPos, enemyPos, false);
	StateInfo * info = getStateInfo(totalDepth, hashVal);

	do {
#ifdef PRINT_LOG
		unsigned long long turnStartTime = getCurTime();
#endif

		enemyMinimax(board, myPos, enemyPos, 0, depthLimitLvl, -INF, INF, hashVal, info, mtx, curTotalDepth);
		mtx.lock();
		if (curTotalDepth == totalDepth){
#ifdef PRINT_LOG
			//cout << "steal depth: " << depthLimitLvl << " time: " << (getCurTime() - turnStartTime) << "ms" << endl;
#endif
			++depthLimitLvl;
		}
		mtx.unlock();
	} while (curTotalDepth == totalDepth && depthLimitLvl <= myUpper + 1);
#ifdef PRINT_LOG
	cout << "Final steal depth: " << (depthLimitLvl - 1) << endl;
#endif

	if (board != NULL){
		delete[] board;
		board = NULL;
	}
}

void mainThreadJob(){
	mtx.lock();
	++totalDepth;
	mtx.unlock();

	AI *p_ai = AI::GetInstance();

	if (totalDepth == 0){
		initZobrist();
		isSecCopyFirst = true;
		if (p_ai->IsMyTurn()){
			isMeFirst = true;
		}
		else {
			isMeFirst = false;
		}
	}

	if (p_ai->IsMyTurn())
	{
#ifdef PRINT_LOG
		unsigned long long startTime = getCurTime();
#endif
		timeOut = false;
		
		thread(sleepThreadJob, totalDepth).detach();

		int * board = p_ai->GetBoard();	// Access block at (x, y) by using board[CONVERT_COORD(x,y)]
		Position myPos = p_ai->GetMyPosition();
		Position enemyPos = p_ai->GetEnemyPosition();

		//check isSecCopyFirst status
		if (isSecCopyFirst && isMeFirst && (enemyPos.x != 10 - myPos.x || enemyPos.y != 10 - myPos.y)){
			isSecCopyFirst = false;
		}
		//end check

		int depthLimitLvl = START_DEPTH_LEVEL;
		int myUpper = getUpperLongest(board, myPos);
		int enemyUpper = getUpperLongest(board, enemyPos);
		int cmd = UNKNOWN_DIRECTION;
		for (int direction = 1; direction <= 4; ++direction){
			Position nextPos = moveDirection(myPos, direction);
			if (inMatrix(nextPos) && board[CONVERT_COORD(nextPos.x, nextPos.y)] == BLOCK_EMPTY){
				cmd = direction;
				break;
			}
		}

		unsigned long long hashVal = 0;
		StateInfo * info = NULL;

		if (isSplitStatus || isSplit(board, myPos, enemyPos)){
			isSplitStatus = true;
		}
		else {
			hashVal = hashBoard(board, myPos, enemyPos, true);
			info = getStateInfo(totalDepth, hashVal);
		}

		do {
#ifdef PRINT_LOG			
			unsigned long long turnStartTime = getCurTime();
#endif
			int tmpCmd;
			if (isSplitStatus){
				tmpCmd = dlsEstLongest(board, myPos, 0, depthLimitLvl, MIN(myUpper, enemyUpper + 1));
			}
			else {
				tmpCmd = minimax(board, myPos, enemyPos, 0, depthLimitLvl, -INF, INF, hashVal, info);
			}
			
			if (timeOut == false){
				Position tmpPos = moveDirection(myPos, tmpCmd);
				if (inMatrix(tmpPos) && board[CONVERT_COORD(tmpPos.x, tmpPos.y)] == BLOCK_EMPTY){
					cmd = tmpCmd;
				}
#ifdef PRINT_LOG
				//cout << "depth: " << depthLimitLvl << " time: " << (getCurTime() - turnStartTime) << "ms" << endl;
#endif
				++depthLimitLvl;
			}
		} while (timeOut == false && depthLimitLvl <= myUpper);
#ifdef PRINT_LOG
		cout << "Final depth: " << (depthLimitLvl - 1) << endl;
		cout << "Time: " << (getCurTime() - startTime) << "ms" << endl;
		cout << "command = " << cmd << endl;
		cout << "Est value = " << ((info != NULL) ? info->estVal : 0) << endl;
#endif

#ifdef COPY_SPECIAL
		if ((!isSplitStatus) && isSpecialBoard(board) && (!isMeFirst) && isSecCopyFirst && info->estVal < STOP_SEARCH_VAL){
			int tmpCmd = whichDirection(myPos, Position(10 - enemyPos.x, 10 - enemyPos.y));
			Position tmpPos = moveDirection(myPos, tmpCmd);
			if (inMatrix(tmpPos) && board[CONVERT_COORD(tmpPos.x, tmpPos.y)] == BLOCK_EMPTY){
				cmd = tmpCmd;
			}
		}
#endif

#ifdef PRINT_TEST
		if (isSplitStatus == true){
			ofstream os("unitTest.txt", ofstream::app);

			for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
				for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
					os << board[CONVERT_COORD(idRow, idCol)] << " ";
				}
				os << endl;
			}
			os << endl;


			int depth = (rand() % 12) + 1;
			os << myPos.x << " " << myPos.y << " " << enemyPos.x << " " << enemyPos.y << " " << depth << endl;
			os << dlsEstLongest_old(board, myPos, 0, depth, 121) << " " << dlsEstLongest_old(board, enemyPos, 0, depth, 121) << " " << endl;
			os.close();
		}
		/*os << heurEstLongest(board, myPos) << " " << heurEstLongest2(board, myPos) << endl;
		if (isSplitStatus == true){
			os << heurEstForSplit(board, myPos, enemyPos, true) << " ";			
		}
		else {
			os << heurEstForNonSplit(board, myPos, enemyPos, true) << " ";
		}
		os << minimax_old(board, myPos, enemyPos, 0, depth, -INF, INF) << endl;*/		
#endif
		
		//Remember to call AI_Move() within allowed time		
		Game::GetInstance()->AI_Move(cmd);
	}
	else
	{
		// Do something while waiting for your opponent
		int * stealBoard = new int[MAP_SIZE * MAP_SIZE];
		memcpy(stealBoard, p_ai->GetBoard(), sizeof(int)* MAP_SIZE * MAP_SIZE);

		thread(stealThreadJob, stealBoard, p_ai->GetMyPosition(), p_ai->GetEnemyPosition(), totalDepth).detach();

		Position myPos = p_ai->GetMyPosition();
		Position enemyPos = p_ai->GetEnemyPosition();

		//check isSecCopyFirst status
		if (isSecCopyFirst && !isMeFirst && (enemyPos.x != 10 - myPos.x || enemyPos.y != 10 - myPos.y)){
			isSecCopyFirst = false;
		}
		//end check

#ifdef PRINT_LOG2
		if (isSecCopyFirst){
			cout << "isSecCopyFirst == true" << endl;
		}
		else {
			cout << "isSecCopyFirst == false" << endl;
		}
#endif
	}
}