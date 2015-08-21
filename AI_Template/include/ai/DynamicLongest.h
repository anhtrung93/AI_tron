#pragma once

#include <ai/AI.h>
#include "LowSupport.h"
#include "HighSupport.h"
#include "StaticLongest.h"
#include "hash.h"
#include "Database.h"
#include "TreeOfChambers.h"
#include <algorithm>
#include <mutex>


extern bool timeOut;
extern int totalDepth;

int heurEstVonoroi(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
	int distFromMe[MAP_SIZE][MAP_SIZE];
	int distFromEnemy[MAP_SIZE][MAP_SIZE];
	calMinDistToAll(board, myPos, distFromMe);
	calMinDistToAll(board, enemyPos, distFromEnemy);

	int myScore = 0, enemyScore = 0;

	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			if (distFromMe[idRow][idCol] < distFromEnemy[idRow][idCol]){
				++myScore;
			}
			else if (distFromMe[idRow][idCol] > distFromEnemy[idRow][idCol]){
				++enemyScore;
			}
			if (distFromMe[idRow][idCol] == distFromEnemy[idRow][idCol] && distFromMe[idRow][idCol] != INF){
				if (isMaxPlayer){
					++myScore;
				}
				else {
					++enemyScore;
				}
			}
		}
	}
	return myScore - enemyScore;
}

int heurEstTreeVonoroi(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
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
	int myScore = getUpperLongest(myVoronoiBoard, myPos);
	int enemyScore = getUpperLongest(enemyVoronoiBoard, enemyPos);

	delete[] myVoronoiBoard;
	delete[] enemyVoronoiBoard;

	return myScore - enemyScore;
}

int heurEstTreeChamber(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
	int distFromMe[MAP_SIZE][MAP_SIZE];
	int distFromEnemy[MAP_SIZE][MAP_SIZE];
	calMinDistToAll(board, myPos, distFromMe);
	calMinDistToAll(board, enemyPos, distFromEnemy);
	int chamber[MAP_SIZE][MAP_SIZE];

	int * myVoronoiBoard = new int[MAP_SIZE * MAP_SIZE];
	int * enemyVoronoiBoard = new int[MAP_SIZE * MAP_SIZE];
	int * vonoroiBoard = new int[MAP_SIZE * MAP_SIZE];
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			chamber[idRow][idCol] = -1;
			if (distFromMe[idRow][idCol] == distFromEnemy[idRow][idCol] && distFromMe[idRow][idCol] == INF){
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				vonoroiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
			}
			else if (distFromMe[idRow][idCol] < distFromEnemy[idRow][idCol]){
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
				vonoroiBoard[CONVERT_COORD(idRow, idCol)] = MY_VONOROI_EMPTY;
			}
			else if (distFromMe[idRow][idCol] > distFromEnemy[idRow][idCol]){
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
				vonoroiBoard[CONVERT_COORD(idRow, idCol)] = ENEMY_VONOROI_EMPTY;
			}
			else if (isMaxPlayer){
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
				vonoroiBoard[CONVERT_COORD(idRow, idCol)] = MY_VONOROI_EMPTY;
			}
			else {
				myVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_OBSTACLE;
				enemyVoronoiBoard[CONVERT_COORD(idRow, idCol)] = BLOCK_EMPTY;
				vonoroiBoard[CONVERT_COORD(idRow, idCol)] = ENEMY_VONOROI_EMPTY;
			}
		}
	}

#ifdef PRINT_LOG2
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			cout << vonoroiBoard[CONVERT_COORD(idRow, idCol)] << " ";
		}
		cout << endl;
	}
	cout << endl;
#endif

	vector<Chamber *> chamberList;
	int myRootChamberId = 0;
	buildChamberTree(myVoronoiBoard, vonoroiBoard, myPos, chamberList, chamber);
	int enemyRootChamberId = (int)chamberList.size();
	buildChamberTree(enemyVoronoiBoard, vonoroiBoard, enemyPos, chamberList, chamber);

#ifdef PRINT_LOG2
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			if (chamber[idRow][idCol] < 0 || chamber[idRow][idCol] > 9){
				cout << chamber[idRow][idCol] << " ";
			}
			else {
				cout << chamber[idRow][idCol] << "  ";
			}
		}
		cout << endl;
	}
	cout << endl;
#endif

	int myMinBattle = INF;
	int myScoreCutOff = -INF;
	int myScoreBattle = -INF;
	for (int idChamber = 0; idChamber < enemyRootChamberId; ++idChamber){
		Chamber * curChamber = chamberList[idChamber];
		if (curChamber != NULL){
			if (curChamber->chamberType == BATTLE_FRONT){
				myMinBattle = MIN(myMinBattle, curChamber->maxSelfArea);
				int curBattleScore = distFromMe[curChamber->rootPos.x][curChamber->rootPos.y] + curChamber->maxSelfArea;
				int totalEnemyBattle = 0;
				int maxBattle = 0;
				if (curChamber->enemies.size() > 1){
					map<int, int>::iterator itEnemies;
					for (itEnemies = curChamber->enemies.begin(); itEnemies != curChamber->enemies.end(); ++itEnemies){
						int tmp = chamberList[itEnemies->first]->selfArea;
						if (itEnemies->second != 1){
							totalEnemyBattle += tmp;
						}
						maxBattle = MAX(tmp, maxBattle);
					}
				}
				myScoreBattle = MAX(myScoreBattle, curBattleScore + totalEnemyBattle - maxBattle);				
			}
			else {
				myScoreCutOff = MAX(myScoreCutOff, distFromMe[curChamber->rootPos.x][curChamber->rootPos.y] + curChamber->maxSelfArea);
			}
		}
	}

	int enemyScoreCutOff = -INF;
	int enemyScoreBattle = -INF;
	int enemyMinBattle = INF;
	for (int idChamber = enemyRootChamberId; idChamber < (int)chamberList.size(); ++idChamber){
		Chamber * curChamber = chamberList[idChamber];
		if (curChamber != NULL){
			if (curChamber->chamberType == BATTLE_FRONT){
				int curBattleScore = distFromEnemy[curChamber->rootPos.x][curChamber->rootPos.y] + curChamber->maxSelfArea;
				int totalEnemyBattle = 0;
				int maxBattle = 0;
				if (curChamber->enemies.size() > 1){
					map<int, int>::iterator itEnemies;
					for (itEnemies = curChamber->enemies.begin(); itEnemies != curChamber->enemies.end(); ++itEnemies){
						int tmp = chamberList[itEnemies->first]->selfArea;
						if (itEnemies->second != 1){
							totalEnemyBattle += tmp;
						}
						maxBattle = MAX(tmp, maxBattle);
					}
				}
				enemyScoreBattle = MAX(enemyScoreBattle, curBattleScore + totalEnemyBattle - maxBattle);
				enemyMinBattle = MIN(enemyMinBattle, curChamber->maxSelfArea);
			}
			else {
				enemyScoreCutOff = MAX(enemyScoreCutOff, distFromEnemy[curChamber->rootPos.x][curChamber->rootPos.y] + curChamber->maxSelfArea);
			}
		}
	}

	myScoreCutOff = (myScoreCutOff == -INF) ? 0 : myScoreCutOff;
	enemyScoreCutOff = (enemyScoreCutOff == -INF) ? 0 : enemyScoreCutOff;

#ifdef PRINT_LOG2
	cout << endl;
	cout << myMinBattle << " " << myScoreCutOff << " " << myScoreBattle << endl;
	cout << enemyMinBattle << " " << enemyScoreCutOff << " " << enemyScoreBattle << endl;
	cout << endl;
#endif

	int result = MIN(myScoreCutOff - (enemyScoreBattle + myMinBattle), myScoreCutOff - enemyScoreCutOff);
	result = MAX(result, MIN(myScoreBattle + enemyMinBattle - enemyScoreCutOff, myScoreBattle - enemyScoreBattle));

	emptyChamberList(chamberList);

	delete[] myVoronoiBoard;
	delete[] enemyVoronoiBoard;
	delete[] vonoroiBoard;

	return result;
}

#ifdef UNIT_TEST
int heurEstForNonSplit(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
	return heurEstTreeVonoroi(board, myPos, enemyPos, isMaxPlayer);
}
#else // if not UNIT_TEST
int heurEstForNonSplit(int * board, const Position & myPos, const Position & enemyPos, bool isMaxPlayer){
#ifdef COPY_SPECIAL
	if (isSpecialBoard(board) && isSecCopyFirst && !isMeFirst){
		return heurEstVonoroi(board, myPos, enemyPos, isMaxPlayer);
	}
#endif
	int tmp = heurEstTreeChamber(board, myPos, enemyPos, isMaxPlayer);
	if (tmp == 0){
		if (isMaxPlayer){
			return -1;
		} else {
			return 1;
		}
	}
	return tmp;
}
#endif //UNIT_TEST

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
#ifndef UNIT_TEST
			if (isSecCopyFirst && isPalinBoard(board, myPos, enemyPos)){
				if (isMaxPlayer){
					return -INF * 3 / 4;
				}
				else {
					return INF * 3 / 4;
				}
			}
#endif
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
	int result = diff + (INF / 2) * ((diff > 0) ? 1 : -1);
#ifndef UNIT_TEST
	if (abs(result) >= STOP_SEARCH_VAL && abs(result) < INF * 3 / 4){
		result += (result > 0) ? 100 : -100;
	}
#endif
	return result;
}

inline void preprocessParam(StateInfo * info, int & alpha, int & beta, bool & processNeg, int depthLvl, int depthLimitLvl){
	if (depthLvl == 0 && info->depthExplore != -1){
		processNeg = (info->estVal <= -STOP_SEARCH_VAL) ? true : false;
	}
}

inline bool isCutNode(StateInfo * info, int alpha, int beta, bool processNeg, int depthLvl, int depthLimitLvl, int & cutResult){
	if (depthLvl == 0 || info->depthExplore == -1){
		return false;
	}
	else  if (info->depthExplore >= depthLimitLvl - depthLvl && processNeg == false){
		cutResult = info->estVal;
		return true;
	} else if (info->depthFromChildren >= depthLimitLvl - depthLvl){
		cutResult = info->estVal;
		return true;
	}
	else {
		if (info->estVal >= STOP_SEARCH_VAL){
			cutResult = info->estVal;
			return true;
		}
		else if (info->estVal <= -STOP_SEARCH_VAL && processNeg == false){
			cutResult = info->estVal;
			return true;
		}
	}
	return false;
}

inline void getOrderedNextMoves(int * board, const Position & posOfMovePlayer, bool isMaxPlayer, uint64 hashVal, int depthLvl, StateInfo * info, vector<pair<int, pair<int, unsigned long long>>> & nextMoves){
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
}

inline void storeStateInfo(StateInfo * info, int result, int alpha, int beta, int depthExplore, int depthChild, bool isSplit){
	info->estVal = result;
	info->depthExplore = depthExplore;
	info->depthFromChildren = depthChild;
	info->isSplit = isSplit;
	//info->depthExplore = depthLimitLvl - depthLvl;
}

int minimax(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, bool processNeg = false){
	bool isMaxPlayer = (depthLvl % 2 == 0) ? true : false;

	if (info->depthExplore == -1){
		info->isSplit = isSplit(board, myPos, enemyPos);
	}

	preprocessParam(info, alpha, beta, processNeg, depthLvl, depthLimitLvl);
	int result;
	if (isCutNode(info, alpha, beta, processNeg, depthLvl, depthLimitLvl, result)){
		return result;
	}

	if (info->isSplit == true){
		if (info->estVal == 0){
			info->estVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
			info->depthExplore = INF / 2;
			info->depthFromChildren = INF / 2;
		}
		return info->estVal;
	}
	else if (depthLvl >= depthLimitLvl){
		info->estVal = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
		info->depthExplore = 0;
		info->depthFromChildren = 0;
		return info->estVal;
	}

	Position posOfMovePlayer = (isMaxPlayer) ? myPos : enemyPos;
	result = (isMaxPlayer) ? -INF : INF;

	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
	getOrderedNextMoves(board, posOfMovePlayer, isMaxPlayer, hashVal, depthLvl, info, nextMoves);

	int directionToMove = UNKNOWN_DIRECTION;
	int depthChildMin = INF;
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
				
		depthChildMin = MIN(depthChildMin, newStateInfo->depthFromChildren);

		if (beta < alpha || (isMaxPlayer && result >= STOP_SEARCH_VAL) || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg) || timeOut == true){
			break;
		}
	}

	if (timeOut == false){
		info->estVal = result;
		info->depthExplore = depthLimitLvl - depthLvl;
		info->depthFromChildren = depthChildMin + 1;
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

int enemyMinimax(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, mutex & mtx, const int curTotalDepth, bool processNeg = false){
	bool isMaxPlayer = (depthLvl % 2 == 0) ? false : true;

	bool tmpSplit = false;
	if (info->depthExplore == -1){
		tmpSplit = isSplit(board, myPos, enemyPos);
	}

	preprocessParam(info, alpha, beta, processNeg, depthLvl, depthLimitLvl);
	int result;
	if (isCutNode(info, alpha, beta, processNeg, depthLvl, depthLimitLvl, result)){
		return result;
	}

	if (info->isSplit || tmpSplit){
		int tmpEstVal = info->estVal;
		if (tmpEstVal == 0){
			tmpEstVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
			mtx.lock();
			if (curTotalDepth == totalDepth){
				info->estVal = tmpEstVal;
				info->isSplit = true;
				info->depthExplore = INF / 2;
				info->depthFromChildren = INF / 2;
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
			info->depthFromChildren = 0;
		}
		mtx.unlock();
		return tmpEstVal;
	}

	Position posOfMovePlayer = (isMaxPlayer) ? myPos : enemyPos;
	result = (isMaxPlayer) ? -INF : INF;

	if (curTotalDepth != totalDepth){
		return 0;
	}

	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
	getOrderedNextMoves(board, posOfMovePlayer, isMaxPlayer, hashVal, depthLvl, info, nextMoves);

	if (curTotalDepth != totalDepth){
		return 0;
	}

	int directionToMove = UNKNOWN_DIRECTION;
	int depthChildMin = INF;
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

		depthChildMin = MIN(depthChildMin, newStateInfo->depthFromChildren);

		if (curTotalDepth != totalDepth){
			return 0;
		}
		else if (beta < alpha || (isMaxPlayer && result >= STOP_SEARCH_VAL) || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg)){
			break;
		}

	}

	mtx.lock();
	if (curTotalDepth == totalDepth){
		info->setValues(result, depthLimitLvl - depthLvl, depthChildMin + 1, tmpSplit);
	}
	mtx.unlock();

	return result;
}


//int minimax_old(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta){
//	bool isMaxPlayer = (depthLvl % 2 == 0) ? true : false;
//
//	if (isSplit(board, myPos, enemyPos) == true){
//		return heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
//	}
//	else if (depthLvl >= depthLimitLvl){
//		return heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
//	}
//
//	Position posOfMovePlayer;
//	int result;
//
//	if (isMaxPlayer == true){
//		posOfMovePlayer = myPos;
//		result = -INF;
//	}
//	else {
//		posOfMovePlayer = enemyPos;
//		result = INF;
//	}
//
//	int directionToMove = UNKNOWN_DIRECTION;
//	for (int direction = 1; direction <= 4; ++direction){
//		Position newPos = moveDirection(posOfMovePlayer, direction);
//		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
//			board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
//			++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
//			if (isMaxPlayer == true){
//				result = MAX(result, minimax_old(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta));
//				alpha = MAX(alpha, result);
//			}
//			else {
//				result = MIN(result, minimax_old(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta));
//				beta = MIN(beta, result);
//			}
//			--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
//			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;
//
//			if (beta < alpha){
//				break;
//			}
//		}
//	}
//
//	return result;
//}
//
//int minimax_mtdf(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, bool processNeg = false){
//	bool isMaxPlayer = (depthLvl % 2 == 0) ? true : false;
//
//	if (info->depthExplore == -1){
//		info->isSplit = isSplit(board, myPos, enemyPos);
//	}
//
//	preprocessParam(info, alpha, beta, processNeg, depthLvl, depthLimitLvl);
//	int result;
//	if (isCutNode(info, alpha, beta, processNeg, depthLvl, depthLimitLvl, result)){
//		return result;
//	}
//
//	if (info->isSplit == true){
//		if (info->estVal == 0){
//			info->estVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
//			info->depthFromChildren = info->depthExplore = INF / 2;
//			info->flag = FLAG_EXACT;
//		}
//		return info->estVal;
//	}
//	else if (depthLvl >= depthLimitLvl){
//		info->estVal = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
//		info->depthFromChildren = info->depthExplore = 0;
//		info->flag = FLAG_EXACT;
//		return info->estVal;
//	}
//
//	Position posOfMovePlayer = (isMaxPlayer) ? myPos : enemyPos;
//	result = (isMaxPlayer) ? -INF : INF;
//	
//	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
//	getOrderedNextMoves(board, posOfMovePlayer, isMaxPlayer, hashVal, depthLvl, info, nextMoves);
//
//	int directionToMove = UNKNOWN_DIRECTION;
//	int depthChildMin = INF;
//	int alphaOrigin = alpha, betaOrigin = beta;
//	for (int idMove = 0; idMove < (int)nextMoves.size(); ++idMove){
//		int direction = nextMoves[idMove].first;
//		unsigned long long newHashVal = nextMoves[idMove].second.second;
//		StateInfo * newStateInfo = info->nextStates[direction];
//		Position newPos = moveDirection(posOfMovePlayer, direction);
//
//		board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
//		++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
//		if (isMaxPlayer == true){
//			int tmp = minimax_mtdf(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, processNeg);
//			if (result < tmp){
//				result = tmp;
//				directionToMove = direction;
//			}
//			alpha = MAX(alpha, result);
//		}
//		else {
//			int tmp = minimax_mtdf(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, processNeg);
//			if (result > tmp){
//				result = tmp;
//			}
//			beta = MIN(beta, result);
//		}
//		--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
//		board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;
//
//		depthChildMin = MIN(depthChildMin, newStateInfo->depthFromChildren);
//
//		if (beta < alpha || (isMaxPlayer && result >= STOP_SEARCH_VAL) || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg) || timeOut == true){
//			break;
//		}
//	}
//
//	if (timeOut == false){
//		storeStateInfo(info, result, alphaOrigin, betaOrigin, depthLimitLvl - depthLvl, depthChildMin + 1, info->isSplit);
//	}
//
//
//#ifndef UNIT_TEST
//	if (depthLvl == 0){
//		return directionToMove;
//	}
//	else {
//		return result;
//	}
//#else
//	return result;
//#endif
//}
//
//int mtdf(int * board, const Position & myPos, const Position & enemyPos, int initValue, int depthLimitLvl, unsigned long long hashVal, StateInfo * info){
//	int g = initValue;
//	int upperBound = INF;
//	int lowerBound = -INF;
//	do {
//		int beta;
//		if (g == lowerBound){
//			beta = g + 1;
//		}
//		else {
//			beta = g;
//		}
//		g = minimax_mtdf(board, myPos, enemyPos, 0, depthLimitLvl, beta - 1, beta, hashVal, info, true);
//		if (g < beta){
//			upperBound = g;
//		}
//		else {
//			lowerBound = g;
//		}
//	} while (lowerBound < upperBound);
//	return g;
//}
//
//int minimax_scout(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, bool processNeg = false){
//	bool isMaxPlayer = (depthLvl % 2 == 0) ? true : false;
//
//	if (info->depthExplore == -1){
//		info->isSplit = isSplit(board, myPos, enemyPos);
//	}
//
//	preprocessParam(info, alpha, beta, processNeg, depthLvl, depthLimitLvl);
//	int result;
//	if (isCutNode(info, alpha, beta, processNeg, depthLvl, depthLimitLvl, result)){
//		return result;
//	}
//
//	if (info->isSplit == true){
//		if (info->estVal == 0){
//			info->estVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
//			info->depthFromChildren = info->depthExplore = INF / 2;
//			info->flag = FLAG_EXACT;
//		}
//		return info->estVal;
//	}
//	else if (depthLvl >= depthLimitLvl){
//		info->estVal = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
//		info->depthFromChildren = info->depthExplore = 0;
//		info->flag = FLAG_EXACT;
//		return info->estVal;
//	}
//
//	Position posOfMovePlayer = (isMaxPlayer) ? myPos : enemyPos;
//	result = (isMaxPlayer) ? -INF : INF;
//
//	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
//	getOrderedNextMoves(board, posOfMovePlayer, isMaxPlayer, hashVal, depthLvl, info, nextMoves);
//
//	int directionToMove = UNKNOWN_DIRECTION;
//	int depthChildMin = INF;
//	int alphaOrigin = alpha, betaOrigin = beta;
//	for (int idMove = 0; idMove < (int)nextMoves.size(); ++idMove){
//		int direction = nextMoves[idMove].first;
//		unsigned long long newHashVal = nextMoves[idMove].second.second;
//		StateInfo * newStateInfo = info->nextStates[direction];
//		Position newPos = moveDirection(posOfMovePlayer, direction);
//
//		board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
//		++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
//		if (isMaxPlayer == true){
//			int tmp = minimax_scout(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, processNeg);
//			if (tmp > alpha && tmp < betaOrigin && idMove > 0){
//				tmp = minimax_scout(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, tmp, betaOrigin, newHashVal, newStateInfo, processNeg);
//			}
//			if (alpha < tmp){
//				alpha = tmp;
//				directionToMove = direction;
//			}
//			beta = alpha + 1;
//			result = alpha;
//		}
//		else {
//			int tmp = minimax_scout(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, processNeg);
//			if (tmp > alphaOrigin && tmp < beta && idMove > 0){
//				tmp = minimax_scout(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alphaOrigin, tmp, newHashVal, newStateInfo, processNeg);
//			}
//			beta = MIN(beta, tmp);
//			alpha = beta - 1;
//			result = beta;
//		}
//		--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
//		board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;
//
//		depthChildMin = MIN(depthChildMin, newStateInfo->depthFromChildren);
//
//		if (timeOut == true){
//			break;
//		} else if (isMaxPlayer){			
//			if (alpha >= betaOrigin || result >= STOP_SEARCH_VAL){
//				break;
//			}
//		}
//		else {			
//			if (beta <= alphaOrigin || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg)){
//				break;
//			}
//		}		
//	}
//
//	if (timeOut == false){
//		storeStateInfo(info, result, alphaOrigin, betaOrigin, depthLimitLvl - depthLvl, depthChildMin + 1, info->isSplit);
//	}
//
//
//#ifndef UNIT_TEST
//	if (depthLvl == 0){
//		return directionToMove;
//	}
//	else {
//		return result;
//	}
//#else
//	return result;
//#endif
//}
//
//int enemyMinimax_scout(int * board, const Position & myPos, const Position & enemyPos, int depthLvl, int depthLimitLvl, int alpha, int beta, unsigned long long hashVal, StateInfo * info, mutex & mtx, const int curTotalDepth, bool processNeg = false){
//	bool isMaxPlayer = (depthLvl % 2 == 0) ? false : true;
//
//	bool tmpSplit = false;
//	if (info->depthExplore == -1){
//		tmpSplit = isSplit(board, myPos, enemyPos);
//	}
//
//	preprocessParam(info, alpha, beta, processNeg, depthLvl, depthLimitLvl);
//	int result;
//	if (isCutNode(info, alpha, beta, processNeg, depthLvl, depthLimitLvl, result)){
//		return result;
//	}
//
//	if (info->isSplit || tmpSplit){
//		int tmpEstVal = info->estVal;
//		if (tmpEstVal == 0){
//			tmpEstVal = heurEstForSplit(board, myPos, enemyPos, isMaxPlayer);
//			mtx.lock();
//			if (curTotalDepth == totalDepth){
//				info->estVal = tmpEstVal;
//				info->isSplit = true;
//				info->depthFromChildren = info->depthExplore = INF / 2;
//			}
//			mtx.unlock();
//		}
//		return tmpEstVal;
//	}
//	else if (depthLvl >= depthLimitLvl){
//		int tmpEstVal = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
//		mtx.lock();
//		if (curTotalDepth == totalDepth){
//			info->estVal = tmpEstVal;
//			info->isSplit = false;
//			info->depthFromChildren = info->depthExplore = 0;
//		}
//		mtx.unlock();
//		return tmpEstVal;
//	}
//
//	Position posOfMovePlayer = (isMaxPlayer) ? myPos : enemyPos;
//	result = (isMaxPlayer) ? -INF : INF;
//
//	if (curTotalDepth != totalDepth){
//		return 0;
//	}
//
//	vector<pair<int, pair<int, unsigned long long>>> nextMoves;
//	getOrderedNextMoves(board, posOfMovePlayer, isMaxPlayer, hashVal, depthLvl, info, nextMoves);
//
//	if (curTotalDepth != totalDepth){
//		return 0;
//	}
//
//	int alphaOrigin = alpha, betaOrigin = beta;
//	int directionToMove = UNKNOWN_DIRECTION;
//	int depthChildMin = INF;
//	for (int idMove = 0; idMove < (int)nextMoves.size(); ++idMove){
//		int direction = nextMoves[idMove].first;
//		unsigned long long newHashVal = nextMoves[idMove].second.second;
//		StateInfo * newStateInfo = info->nextStates[direction];
//		Position newPos = moveDirection(posOfMovePlayer, direction);
//
//		board[CONVERT_COORD(newPos.x, newPos.y)] = board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];
//		++board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it trails
//		if (isMaxPlayer == true){
//			int tmp = enemyMinimax_scout(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, mtx, curTotalDepth, processNeg);
//			if (tmp > alpha && tmp < betaOrigin && idMove > 0){
//				tmp = enemyMinimax_scout(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, tmp, betaOrigin, newHashVal, newStateInfo, mtx, curTotalDepth, processNeg);
//			}
//			if (alpha < tmp){
//				alpha = tmp;
//				directionToMove = direction;
//			}
//			beta = alpha + 1;
//			result = alpha;
//		}
//		else {
//			int tmp = enemyMinimax_scout(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta, newHashVal, newStateInfo, mtx, curTotalDepth, processNeg);
//			if (tmp > alphaOrigin && tmp < beta && idMove > 0){
//				tmp = enemyMinimax_scout(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alphaOrigin, tmp, newHashVal, newStateInfo, mtx, curTotalDepth, processNeg);
//			}
//			beta = MIN(beta, tmp);
//			alpha = beta - 1;
//			result = beta;
//		}
//		--board[CONVERT_COORD(posOfMovePlayer.x, posOfMovePlayer.y)];//Make it normal
//		board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;
//
//		depthChildMin = MIN(depthChildMin, newStateInfo->depthFromChildren);
//
//		if (curTotalDepth != totalDepth){
//			return 0;
//		}
//		else if (isMaxPlayer){
//			if (alpha >= betaOrigin || result >= STOP_SEARCH_VAL){
//				break;
//			}
//		}
//		else {
//			if (beta <= alphaOrigin || (!isMaxPlayer && result <= -STOP_SEARCH_VAL && !processNeg)){
//				break;
//			}
//		}
//	}
//
//	mtx.lock();
//	if (curTotalDepth == totalDepth){
//		storeStateInfo(info, result, alphaOrigin, betaOrigin, depthLimitLvl - depthLvl, depthChildMin + 1, info->isSplit);
//	}
//	mtx.unlock();
//
//	return result;
//}