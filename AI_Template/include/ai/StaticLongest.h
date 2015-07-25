#pragma once

#include <ai/AI.h>
#include "LowSupport.h"
#include "HighSupport.h"
#include <list>
#include <vector>

using namespace std;

//NOTE: O(n)
int heurEstLongest(int * board, const Position & myPos, bool isFirst = true, list<Position> & restoreList = list<Position>()){
	int myID;
	if (isFirst == true){
		myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	}

	board[CONVERT_COORD(myPos.x, myPos.y)] = BLOCK_OCCUPIED;
	restoreList.push_back(myPos);

	vector<pair<int, int> > nextMoves;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(myPos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			nextMoves.push_back(pair<int, int>(direction, getConnection(board, newPos)));
		}
	}

	sort(nextMoves.begin(), nextMoves.end(), comparePairMinSecond);

	int result = 1;
	for (vector<pair<int, int> >::iterator itMove = nextMoves.begin(); itMove != nextMoves.end(); ++itMove){
		int direction = itMove->first;
		Position newPos = moveDirection(myPos, direction);
		if (board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			int tmp = heurEstLongest(board, newPos, false, restoreList);
			if (tmp + 1 > result){
				result = tmp + 1;
			}
		}
	}

	if (isFirst == true){
		restoreBoard(board, restoreList);
		board[CONVERT_COORD(myPos.x, myPos.y)] = myID;
	}
	return result;
}

//NOTE: O(n^2)
int heurEstLongest2(int * board, const Position & myPos, bool isFirst = true, list<Position> & restoreList = list<Position>()){
	int myID;
	if (isFirst == true){
		myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	}

	board[CONVERT_COORD(myPos.x, myPos.y)] = BLOCK_OCCUPIED;
	restoreList.push_back(myPos);

	vector<pair<int, pair<int, int> > > nextMoves;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(myPos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			nextMoves.push_back(pair<int, pair<int, int> >(direction, pair<int, int>(getUpperLongest(board, newPos), heurEstLongest(board, newPos))));
		}
	}

	sort(nextMoves.begin(), nextMoves.end(), comparePairPairMaxSecond);

	int result = 1;
	if (nextMoves.size() >= 1){
		int direction = nextMoves.begin()->first;
		pair<int, int> bound = nextMoves.begin()->second;
		if (bound.first == bound.second){
			result = bound.first + 1;
		}
		else {
			Position newPos = moveDirection(myPos, direction);
			result = heurEstLongest2(board, newPos, false, restoreList) + 1;
		}
	}

	if (isFirst == true){
		restoreBoard(board, restoreList);
		board[CONVERT_COORD(myPos.x, myPos.y)] = myID;
	}
	return result;
}

int dlsEstLongest(int * board, const Position & myPos, int depthLvl, int depthLimitLvl, int depthToCut){
	if (depthLvl >= depthLimitLvl){
		return heurEstLongest2(board, myPos);
	}

	int myID = board[CONVERT_COORD(myPos.x, myPos.y)];
	++board[CONVERT_COORD(myPos.x, myPos.y)];//Make it trail
	int result = 1;
	int directionToMove = UNKNOWN_DIRECTION;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(myPos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			board[CONVERT_COORD(newPos.x, newPos.y)] = myID;

			int tmp = dlsEstLongest(board, newPos, depthLvl + 1, depthLimitLvl, depthToCut - 1);
			board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_EMPTY;
			if (tmp + 1 > result){
				result = tmp + 1;
				directionToMove = direction;
			}
			if (result >= depthToCut || timeOut){
				break;
			}
		}
	}
	--board[CONVERT_COORD(myPos.x, myPos.y)];//make it not trail

	if (depthLvl == 0){
		return directionToMove;
	}
	else {
		return result;
	}
}