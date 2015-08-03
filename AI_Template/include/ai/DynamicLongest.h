#pragma once

#include <ai/AI.h>
#include "LowSupport.h"
#include "HighSupport.h"
#include "StaticLongest.h"
#include "hash.h"
#include "Database.h"
#include <algorithm>
#include <mutex>

//TODO turn this off
//#define UNIT_TEST

extern bool timeOut;
extern int totalDepth;

int heurEstForNonSplit(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
	int distFromMe[MAP_SIZE][MAP_SIZE];
	int distFromEnemy[MAP_SIZE][MAP_SIZE];
	calMinDistToAll(board, myPos, distFromMe);
	calMinDistToAll(board, enemyPos, distFromEnemy);

	int * myVoronoiBoard = new int[MAP_SIZE * MAP_SIZE];
	int * enemyVoronoiBoard = new int[MAP_SIZE * MAP_SIZE];
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			if (distFromMe[idRow][idCol] == distFromEnemy[idRow][idCol] && distFromMe[idRow][idCol] == INF){
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
			}
			else if (distFromMe[idRow][idCol] < distFromEnemy[idRow][idCol]){
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
			}
			else if (distFromMe[idRow][idCol] > distFromEnemy[idRow][idCol]){
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
			}
			else if (isMaxPlayer){
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
			}
			else {
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
			}
		}
	}
	int tmp1 = getUpperLongest(myVoronoiBoard, myPos);
	int tmp2 = getUpperLongest(enemyVoronoiBoard, enemyPos);

	delete[] myVoronoiBoard;
	delete[] enemyVoronoiBoard;

	return tmp1 - tmp2;
}

int heurEstForSplit(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
	int diff = 0;
	int enemyHeurLen = heurEstLongest(board, enemyPos);
	int myArea = getUpperLongest(board, myPos);

	if (myArea < enemyHeurLen || (myArea == enemyHeurLen && isMaxPlayer == true)){
		diff = myArea - enemyHeurLen;
		diff -= INF / 4;
	}
	else {
		int myHeurLen = heurEstLongest(board, myPos);
		int enemyArea = getUpperLongest(board, enemyPos);
		if (enemyArea < myHeurLen || (enemyArea == myHeurLen && isMaxPlayer == false)){
			diff = myHeurLen - enemyArea;
			diff += INF / 4;
		}
		else if (myHeurLen == myArea && enemyHeurLen == enemyArea){
			diff = myHeurLen - enemyHeurLen;
			diff += (diff > 0 || (diff == 0 && isMaxPlayer == false)) ? INF / 4 : -INF / 4;
		}
		else {//if (depthLimitLvl - depthLvl - 1 === 0){      
			myHeurLen = heurEstLongest2(board, myPos);
			if (enemyArea < myHeurLen || (enemyArea == myHeurLen && isMaxPlayer == false)){
				diff = myHeurLen - enemyArea;
				diff += INF / 4;
			}
			else {
				enemyHeurLen = heurEstLongest2(board, enemyPos);
				diff = myHeurLen - enemyHeurLen;
				if (myArea < enemyHeurLen || (myArea == enemyHeurLen && isMaxPlayer == true)){
					diff -= INF / 4;
				}
				else if (myHeurLen == myArea && enemyHeurLen == enemyArea){
					diff += (diff > 0 || (diff == 0 && isMaxPlayer == false)) ? INF / 4 : -INF / 4;
				}
			}
		}
	}

	if (diff == 0) {
		diff = ((isMaxPlayer == true) ? -1 : 1);
	}
	return diff + (INF / 2) * ((diff > 0) ? 1 : -1);
}

int minimax_old(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta){
	bool isMaxPlayer = (depthLvl % 2 == 0) ? true : false;

	if (isSplit(board, myPos, enemyPos) == true){
		return heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
	}
	else if (depthLvl >= depthLimitLvl){
		return heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
	}

	Position posOfMovePlayer;
	int result;

	if (isMaxPlayer == true){
		posOfMovePlayer = myPos;
		result = -INF;
	}
	else {
		posOfMovePlayer = enemyPos;
		result = INF;
	}

	int directionToMove = UNKNOWN_DIRECTION;
	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(posOfMovePlayer, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
			++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
			if (isMaxPlayer == true){
				result = MAX(result, minimax_old(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta));
				alpha = MAX(alpha, result);
			}
			else {
				result = MIN(result, minimax_old(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta));
				beta = MIN(beta, result);
			}
			--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;

			if (beta < alpha){
				break;
			}
		}
	}

	return result;
}


int minimax(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, bool processNeg = true){
	bool isMaxPlayer = (depthLvl % 2 == 0) ? true : false;

	if (info->depthExplore == -1){
		info->isSplit = isSplit(board, myPos, enemyPos);
	}
	else if (depthLvl != 0){
		if (info->depthExplore >= depthLimitLvl - depthLvl || info->estVal >= STOP_SEARCH_VAL || (info->estVal <= -STOP_SEARCH_VAL && processNeg == false)){
			return info->estVal;
		}
	}
	else {//depthLvl == 0 and info->depthExplore != -1
		processNeg = (info->estVal <= -STOP_SEARCH_VAL) ? true : false;
	}

	if (info->isSplit == true){
		if (info->estVal == 0){
			info->estVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
			info->depthExplore = 0;
		}
		return info->estVal;
	}
	else if (depthLvl >= depthLimitLvl){
		info->estVal = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
		info->depthExplore = 0;
		return info->estVal;
	}

	Position posOfMovePlayer;
	int result;

	if (isMaxPlayer == true){
		posOfMovePlayer = myPos;
		result = -INF;
	}
	else {
		posOfMovePlayer = enemyPos;
		result = INF;
	}

	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(posOfMovePlayer, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			unsigned long long newHashVal = hashMove(hashVal, posOfMovePlayer, newPos, isMaxPlayer);
			if (info->nextStates[direction] == NULL){
				info->nextStates[direction] = getStateInfo(totalDepth + depthLvl + 1, newHashVal);
			}
			StateInfo * newStateInfo = info->nextStates[direction];

			pair<int, pair<int, unsigned long long>> tmp3(direction, pair<int, unsigned long long>(newStateInfo->estVal, newHashVal));
			nextMoves.push_back(tmp3);
		}
	}

	if (isMaxPlayer){
		sort(nextMoves.begin(), nextMoves.end(), compareSuperPairMaxSecond);
	}
	else {
		sort(nextMoves.begin(), nextMoves.end(), compareSuperPairMinSecond);
	}

	int directionToMove = UNKNOWN_DIRECTION;
	for (int idMove = 0; idMove < (int)nextMoves.size(); ++idMove){
		int direction = nextMoves[idMove].first;
		unsigned long long newHashVal = nextMoves[idMove].second.second;
		StateInfo * newStateInfo = info->nextStates[direction];
		Position newPos = moveDirection(posOfMovePlayer, direction);

		board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
		++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
		if (isMaxPlayer == true){
			int tmp = minimax(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, processNeg);
			if (result < tmp){
				result = tmp;
				directionToMove = direction;
			}
			alpha = MAX(alpha, result);
		}
		else {
			int tmp = minimax(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, processNeg);
			if (result > tmp){
				result = tmp;
			}
			beta = MIN(beta, result);
		}
		--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
		board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;

		if (beta < alpha || (isMaxPlayer && result >= STOP_SEARCH_VAL) || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg) || timeOut == true){
			break;
		}
	}

	if (timeOut == false){
		info->estVal = result;
		info->depthExplore = depthLimitLvl - depthLvl;
	}


#ifndef UNIT_TEST
	if (depthLvl == 0){
		return directionToMove;
	}
	else {
		return result;
	}
#else
	return result;
#endif
}

int enemyMinimax(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, mutex & mtx, const int curTotalDepth, bool processNeg = true){
	bool isMaxPlayer = (depthLvl % 2 == 0) ? false : true;

	bool tmpSplit = false;
	if (info->depthExplore == -1){
		tmpSplit = isSplit(board, myPos, enemyPos);
	}
	else if (depthLvl != 0){
		if (info->depthExplore >= depthLimitLvl - depthLvl || info->estVal >= STOP_SEARCH_VAL || (info->estVal <= -STOP_SEARCH_VAL && processNeg == false)){
			return info->estVal;
		}
	}
	else {//depthLvl == 0 and info->depthExplore != -1
		processNeg = (info->estVal <= -STOP_SEARCH_VAL) ? true : false;
	}

	if (info->isSplit || tmpSplit){
		int tmpEstVal = info->estVal;
		if (tmpEstVal == 0){
			tmpEstVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
			mtx.lock();
			if (curTotalDepth == totalDepth){
				info->estVal = tmpEstVal;
				info->isSplit = true;
				info->depthExplore = 0;
			}
			mtx.unlock();
		}
		return tmpEstVal;
	}
	else if (depthLvl >= depthLimitLvl){
		int tmpEstVal = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
		mtx.lock();
		if (curTotalDepth == totalDepth){
			info->estVal = tmpEstVal;
			info->isSplit = false;
			info->depthExplore = 0;
		}
		mtx.unlock();
		return tmpEstVal;
	}

	Position posOfMovePlayer;
	int result;

	if (isMaxPlayer == true){
		posOfMovePlayer = myPos;
		result = -INF;
	}
	else {
		posOfMovePlayer = enemyPos;
		result = INF;
	}

	if (curTotalDepth != totalDepth){
		return 0;
	}

	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(posOfMovePlayer, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			unsigned long long newHashVal = hashMove(hashVal, posOfMovePlayer, newPos, isMaxPlayer);
			if (info->nextStates[direction] == NULL){
				mtx.lock();
				if (curTotalDepth == totalDepth){
					info->nextStates[direction] = getStateInfo(totalDepth + depthLvl + 1, newHashVal);
				}
				mtx.unlock();
			}
			StateInfo * newStateInfo = info->nextStates[direction];
			if (curTotalDepth != totalDepth || newStateInfo == NULL){
				return 0;
			}

			pair<int, pair<int, unsigned long long>> tmp3(direction, pair<int, unsigned long long>(newStateInfo->estVal, newHashVal));
			nextMoves.push_back(tmp3);
		}
	}

	if (isMaxPlayer){
		sort(nextMoves.begin(), nextMoves.end(), compareSuperPairMaxSecond);
	}
	else {
		sort(nextMoves.begin(), nextMoves.end(), compareSuperPairMinSecond);
	}

	if (curTotalDepth != totalDepth){
		return 0;
	}

	int directionToMove = UNKNOWN_DIRECTION;
	for (int idMove = 0; idMove < (int)nextMoves.size(); ++idMove){
		int direction = nextMoves[idMove].first;
		unsigned long long newHashVal = nextMoves[idMove].second.second;
		StateInfo * newStateInfo = info->nextStates[direction];
		Position newPos = moveDirection(posOfMovePlayer, direction);

		board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
		++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
		if (isMaxPlayer == true){
			int tmp = enemyMinimax(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, mtx, curTotalDepth, processNeg);
			if (result < tmp){
				result = tmp;
				directionToMove = direction;
			}
			alpha = MAX(alpha, result);
		}
		else {
			int tmp = enemyMinimax(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, mtx, curTotalDepth, processNeg);
			if (result > tmp){
				result = tmp;
			}
			beta = MIN(beta, result);
		}
		--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
		board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;

		if (curTotalDepth != totalDepth){
			return 0;
		}
		else if (beta < alpha || (isMaxPlayer && result >= STOP_SEARCH_VAL) || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg)){
			break;
		}

	}

	mtx.lock();
	if (curTotalDepth == totalDepth){
		info->estVal = result;
		info->depthExplore = depthLimitLvl - depthLvl;
		info->isSplit = tmpSplit;
	}
	mtx.unlock();

	return result;
}