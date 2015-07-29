#pragma once

#include <time.h>
#include <thread>
#include <mutex>
#include <ai/DynamicLongest.h>

extern bool timeOut;
extern int totalDepth;
extern bool isSplitStatus;

mutex mtx;

void sleepThreadJob(int depth){
#ifdef PRINT_LOG
	cout << "Start sleep at depth = " << depth << endl;
#endif
	this_thread::sleep_for(chrono::milliseconds(MAX_TIME));
	mtx.lock();
	if (depth == totalDepth){
		timeOut = true;
	}
	mtx.unlock();
#ifdef PRINT_LOG
	cout << "Wake up" << endl;
#endif
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

	if (isSplitStatus == true || totalDepth != curTotalDepth){
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
			cout << "steal depth: " << depthLimitLvl << " time: " << (getCurTime() - turnStartTime) << "ms" << endl;
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
	if (totalDepth == 0){
		initZobrist();
	}

	AI *p_ai = AI::GetInstance();
	if (p_ai->IsMyTurn())
	{
#ifdef PRINT_LOG
		cout << "Set timeOut = false at depth = " << totalDepth << endl;
		unsigned long long startTime = getCurTime();
#endif
		timeOut = false;
		
		thread(sleepThreadJob, totalDepth).detach();

		int * board = p_ai->GetBoard();	// Access block at (x, y) by using board[CONVERT_COORD(x,y)]
		Position myPos = p_ai->GetMyPosition();
		Position enemyPos = p_ai->GetEnemyPosition();

		int depthLimitLvl = START_DEPTH_LEVEL;
		int myUpper = getUpperLongest(board, myPos);
		int enemyUpper = getUpperLongest(board, enemyPos);
		int cmd = UNKNOWN_DIRECTION;

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
				cmd = tmpCmd;
#ifdef PRINT_LOG
				cout << "depth: " << depthLimitLvl << " time: " << (getCurTime() - turnStartTime) << "ms" << endl;
#endif
				++depthLimitLvl;
			}
		} while (timeOut == false && depthLimitLvl <= myUpper);
#ifdef PRINT_LOG
		cout << "Final depth: " << (depthLimitLvl - 1) << endl;
		cout << "Time: " << (getCurTime() - startTime) << "ms" << endl;
		cout << "command = " << cmd << endl;
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
	}
}