#pragma once

#include <ai/AI.h>
#include "LowSupport.h"
#include <list>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

bool isSplit_A_star(int * board, const Position & myPos, const Position & enemyPos){
	int myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	vector<pair<Position, pair<int, int>>> myHeap;
	list<Position> restoreList;

	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(myPos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
			myHeap.push_back(pair<Position, pair<int, int>>(newPos, pair<int, int>(1, 1 + abs(enemyPos.x - newPos.x) + abs(enemyPos.y - newPos.y))));
			push_heap(myHeap.begin(), myHeap.end(), compairPosMinSecondSecond);
			restoreList.push_back(newPos);
		}
	}

	bool result = true;
	while (myHeap.empty() == false && result != false){
		Position pos = myHeap.front().first;
		int distToPos = myHeap.front().second.first;
		pop_heap(myHeap.begin(), myHeap.end(), compairPosMinSecondSecond);
		myHeap.pop_back();

		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(pos, direction);
			if (inMatrix(newPos)){
				if (newPos.x == enemyPos.x && newPos.y == enemyPos.y){
					result = false;
					break;
				}
				else if (board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
					board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
					myHeap.push_back(pair<Position, pair<int, int>>(newPos,
						pair<int, int>(distToPos + 1, distToPos + 1 + abs(enemyPos.x - newPos.x) + abs(enemyPos.y - newPos.y))));
					push_heap(myHeap.begin(), myHeap.end(), compairPosMinSecondSecond);
					restoreList.push_back(newPos);
				}
			}
		}
	}

	restoreBoard(board, restoreList);
	board[CONVERT_COORD(myPos.x, myPos.y)] = myID;

	return result;
};

bool isSplit(int * board, const Position & myPos, const Position & enemyPos){
	int myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	stack<Position> myStack;
	list<Position> restoreList;

	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(myPos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
			myStack.push(newPos);
			restoreList.push_back(newPos);
		}
	}

	bool result = true;
	while (myStack.empty() == false && result != false){
		Position pos = myStack.top();
		myStack.pop();

		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(pos, direction);
			if (inMatrix(newPos)){
				if (newPos.x == enemyPos.x && newPos.y == enemyPos.y){
					result = false;
					break;
				}
				else if (board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
					board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
					myStack.push(newPos);
					restoreList.push_back(newPos);
				}
			}
		}
	}

	restoreBoard(board, restoreList);
	board[CONVERT_COORD(myPos.x, myPos.y)] = myID;

	return result;
};

bool isSplit_bfs(int * board, const Position & myPos, const Position & enemyPos){
	int myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	queue<Position> myStack;
	list<Position> restoreList;

	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(myPos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
			myStack.push(newPos);
			restoreList.push_back(newPos);
		}
	}

	bool result = true;
	while (myStack.empty() == false && result != false){
		Position pos = myStack.front();
		myStack.pop();

		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(pos, direction);
			if (inMatrix(newPos)){
				if (newPos.x == enemyPos.x && newPos.y == enemyPos.y){
					result = false;
					break;
				}
				else if (board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
					board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
					myStack.push(newPos);
					restoreList.push_back(newPos);
				}
			}
		}
	}

	restoreBoard(board, restoreList);
	board[CONVERT_COORD(myPos.x, myPos.y)] = myID;

	return result;
};

void dfsCutVertices(int * board, const Position & pos, int & labelCount, int label[][MAP_SIZE], int lowReach[][MAP_SIZE], int cutVertices[][MAP_SIZE]){
	++labelCount;
	label[pos.x][pos.y] = labelCount;
	lowReach[pos.x][pos.y] = INF;
	cutVertices[pos.x][pos.y] = NORMAL_VERTICE;

	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(pos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			if (label[newPos.x][newPos.y] == INF){
				dfsCutVertices(board, newPos, labelCount, label, lowReach, cutVertices);
				if (lowReach[newPos.x][newPos.y] >= label[pos.x][pos.y]){
					cutVertices[pos.x][pos.y] = CUT_VERTICE;
				}
				lowReach[pos.x][pos.y] = MIN(lowReach[pos.x][pos.y], lowReach[newPos.x][newPos.y]);
			}
			else {
				lowReach[pos.x][pos.y] = MIN(lowReach[pos.x][pos.y], label[newPos.x][newPos.y]);
			}
		}
	}
}

void findCutVertices(int * board, const Position & rootPos, int label[][MAP_SIZE], int lowReach[][MAP_SIZE], int cutVertices[][MAP_SIZE]){
	int labelCount = 0;

	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			label[idRow][idCol] = INF;
			cutVertices[idRow][idCol] = 0;
		}
	}

	dfsCutVertices(board, rootPos, labelCount, label, lowReach, cutVertices);
}

void dfsUpperLongest_old(int * board, const Position & pos, DfsUpperObj * result, int & labelCount, int label[][MAP_SIZE], int lowReach[][MAP_SIZE], int cutVertices[][MAP_SIZE]){
	++labelCount;
	label[pos.x][pos.y] = labelCount;
	lowReach[pos.x][pos.y] = INF;
	cutVertices[pos.x][pos.y] = 0;

	int numRed, numBlack;
	bool isRed;
	if (isPosRed(pos)){
		numRed = 1;
		numBlack = 0;
		isRed = true;
	}
	else {
		numRed = 0;
		numBlack = 1;
		isRed = false;
	}
	result->pos = pos;
	result->isRed = isRed;
	result->numRed = numRed;
	result->numBlack = numBlack;

	vector<DfsUpperObj *> compareInfo;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(pos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			if (label[newPos.x][newPos.y] == INF){
				DfsUpperObj * tmp = new DfsUpperObj();
				dfsUpperLongest_old(board, newPos, tmp, labelCount, label, lowReach, cutVertices);
				compareInfo.push_back(tmp);
				if (lowReach[newPos.x][newPos.y] >= label[pos.x][pos.y]){
					cutVertices[pos.x][pos.y] = 1;
				}
				lowReach[pos.x][pos.y] = MIN(lowReach[pos.x][pos.y], lowReach[newPos.x][newPos.y]);
			}
			else {
				lowReach[pos.x][pos.y] = MIN(lowReach[pos.x][pos.y], label[newPos.x][newPos.y]);
			}
		}
	}

	CutInfo * curCutInfo = NULL;
	if (cutVertices[pos.x][pos.y] == 1){
		curCutInfo = new CutInfo(isRed, 1);
	}

	for (vector<DfsUpperObj *>::iterator id = compareInfo.begin(); id != compareInfo.end(); ++id){
		DfsUpperObj * curObj = *id;
		if (cutVertices[pos.x][pos.y] == 0 || lowReach[curObj->pos.x][curObj->pos.y] < label[pos.x][pos.y]){
			result->numRed += curObj->numRed;
			result->numBlack += curObj->numBlack;
			result->nextCuts.splice(result->nextCuts.end(), curObj->nextCuts);
		}
		else {// when pos is cutVertices
			int numRedTemp = ((isRed) ? 1 : 0) + curObj->numRed;
			int numBlackTemp = ((isRed == false) ? 1 : 0) + curObj->numBlack;
#pragma warning(suppress: 6011)
			curCutInfo->area = MAX(maxAreaBasedOnRedBlack(numRedTemp, numBlackTemp, isRed), curCutInfo->area);

			list<CutInfo *>::iterator itNextCuts;
			for (itNextCuts = curObj->nextCuts.begin(); itNextCuts != curObj->nextCuts.end(); ++itNextCuts){
				int tempArea = maxAreaBasedOnRedBlack(numRedTemp, numBlackTemp, isRed, (*itNextCuts)->isRed) - 1 + (*itNextCuts)->area;
				curCutInfo->area = MAX(tempArea, curCutInfo->area);
			}
		}
		if (*id != NULL){
			delete *id;
			*id = NULL;
		}

	}

	if (curCutInfo != NULL){
		result->nextCuts.push_front(curCutInfo);
	}
}

int getUpperLongest_old(int * board, const Position & myPos){
	int label[MAP_SIZE][MAP_SIZE];
	int lowReach[MAP_SIZE][MAP_SIZE];
	int cutVertices[MAP_SIZE][MAP_SIZE];
	int labelCount = 0;

	int myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	board[CONVERT_COORD(myPos.x, myPos.y)] = BLOCK_EMPTY;

	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			label[idRow][idCol] = INF;
		}
	}

	DfsUpperObj * tempResult = new DfsUpperObj;
	dfsUpperLongest_old(board, myPos, tempResult, labelCount, label, lowReach, cutVertices);

	board[CONVERT_COORD(myPos.x, myPos.y)] = myID;

	int result;
	if (tempResult->nextCuts.empty()){
		result = 1;
	}
	else {
		result = tempResult->nextCuts.front()->area;
	}

	if (tempResult != NULL){
		delete tempResult;
		tempResult = NULL;
	}

	return result;
}

int dfsUpperLongest(int * board, const Position & pos, int label[][MAP_SIZE], int lowReach[][MAP_SIZE], int cutVertices[][MAP_SIZE], list<Position> & restoreList){
	int numRed = 0, numBlack = 0;
	bool isCurRed = isPosRed(pos);
	int maxArea = 1;

	board[CONVERT_COORD(pos.x, pos.y)] = BLOCK_OCCUPIED;
	restoreList.push_back(pos);
	vector<Position> cutPos;

	stack<Position> dfsStack;
	dfsStack.push(pos);
	while (!dfsStack.empty()){
		Position curPos = dfsStack.top();
		dfsStack.pop();
		if (isPosRed(curPos)){
			++numRed;
		}
		else {
			++numBlack;
		}
		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(curPos, direction);
			if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
				if (cutVertices[curPos.x][curPos.y] == CUT_VERTICE && lowReach[newPos.x][newPos.y] >= label[curPos.x][curPos.y]){
					cutPos.push_back(newPos);
				}
				else {
					board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
					restoreList.push_back(newPos);
					dfsStack.push(newPos);
				}
			}
		}
	}

	maxArea = MAX(maxAreaBasedOnRedBlack(numRed, numBlack, isCurRed), maxArea);
	vector<Position>::iterator itCutList;
	for (itCutList = cutPos.begin(); itCutList != cutPos.end(); ++itCutList){
		if (board[CONVERT_COORD(itCutList->x, itCutList->y)] == BLOCK_EMPTY){
			int childResult = dfsUpperLongest(board, *itCutList, label, lowReach, cutVertices, restoreList);
			int tmpArea = maxAreaBasedOnRedBlack(numRed, numBlack, isCurRed, !isPosRed(*itCutList)) + childResult;
			maxArea = MAX(tmpArea, maxArea);
		}
	}

	return maxArea;
}

int getUpperLongest(int * board, const Position & rootPos){
	int myID = board[CONVERT_COORD(rootPos.x, rootPos.y)];
	board[CONVERT_COORD(rootPos.x, rootPos.y)] = BLOCK_EMPTY;

	int label[MAP_SIZE][MAP_SIZE];
	int lowReach[MAP_SIZE][MAP_SIZE];
	int cutVertices[MAP_SIZE][MAP_SIZE];
	findCutVertices(board, rootPos, label, lowReach, cutVertices);

	list<Position> restoreList;
	int result = dfsUpperLongest(board, rootPos, label, lowReach, cutVertices, restoreList);
	restoreBoard(board, restoreList);

	board[CONVERT_COORD(rootPos.x, rootPos.y)] = myID;

	return result;
}

void calMinDistToAll(int * board, const Position & pos, int dist[][MAP_SIZE]){
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			dist[idRow][idCol] = INF;
		}
	}

	queue<Position> myQueue;
	myQueue.push(pos);

	dist[pos.x][pos.y] = 0;
	while (!myQueue.empty()){
		Position curPos = myQueue.front();
		myQueue.pop();
		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(curPos, direction);
			if (inMatrix(newPos) && dist[newPos.x][newPos.y] == INF){
				if (board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){//empty
					dist[newPos.x][newPos.y] = dist[curPos.x][curPos.y] + 1;
					myQueue.push(newPos);
				}
				else if (board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_PLAYER_2 +
					BLOCK_PLAYER_1 - board[CONVERT_COORD(pos.x, pos.y)]){//enemy
					dist[newPos.x][newPos.y] = dist[curPos.x][curPos.y] + 1;
				}
			}
		}
	}
}

