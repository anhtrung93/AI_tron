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
	//cout << "Start sleep at depth = " << depth << endl;
	this_thread::sleep_for(chrono::milliseconds(MAX_TIME));
	mtx.lock();
	if (depth == totalDepth){
		timeOut = true;
	}
	mtx.unlock();
	//cout << "Wake up" << endl;
}

void stealThreadJob(int * board, Position myPos, Position enemyPos, int curTotalDepth){
	//unsigned long long startDelTime = getCurTime();
	removeDatabaseAtDepth(curTotalDepth - 2);
	removeDatabaseAtDepth(curTotalDepth - 1);
	//cout << "Delete time = " << (getCurTime() - startDelTime) << "ms" << endl;

	if (isSplitStatus == true || totalDepth != curTotalDepth){
		return;
	}
	//cout << "Start steal depth at " << curTotalDepth << endl;

	int depthLimitLvl = START_DEPTH_LEVEL;
	int myUpper = getUpperLongest(board, myPos);

	unsigned long long hashVal = hashBoard(board, myPos, enemyPos, false);
	StateInfo * info = getStateInfo(totalDepth, hashVal);

	do {
		//unsigned long long turnStartTime = getCurTime();

		enemyMinimax(board, myPos, enemyPos, 0, depthLimitLvl, -INF, INF, hashVal, info, mtx, curTotalDepth);
		mtx.lock();
		if (curTotalDepth == totalDepth){
			//cout << "steal depth: " << depthLimitLvl << " time: " << (getCurTime() - turnStartTime) << "ms" << endl;
			++depthLimitLvl;
		}
		mtx.unlock();
	} while (curTotalDepth == totalDepth && depthLimitLvl <= myUpper + 1);
	//cout << "Final steal depth: " << (depthLimitLvl - 1) << endl;

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
		//cout << "Set timeOut = false at depth = " << totalDepth << endl;
		timeOut = false;
		//unsigned long long startTime = getCurTime();
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
			//unsigned long long turnStartTime = getCurTime();
			int tmpCmd;
			if (isSplitStatus){
				tmpCmd = dlsEstLongest(board, myPos, 0, depthLimitLvl, MIN(myUpper, enemyUpper + 1));
			}
			else {
				tmpCmd = minimax(board, myPos, enemyPos, 0, depthLimitLvl, -INF, INF, hashVal, info);
			}
			if (timeOut == false){
				cmd = tmpCmd;
				//cout << "depth: " << depthLimitLvl << " time: " << (getCurTime() - turnStartTime) << "ms" << endl;
				++depthLimitLvl;
			}
		} while (timeOut == false && depthLimitLvl <= myUpper);
		//cout << "Final depth: " << (depthLimitLvl - 1) << endl;
		//cout << "Time: " << (getCurTime() - startTime) << "ms" << endl;
		//cout << "command = " << cmd << endl;

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