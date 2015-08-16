#pragma once

#include "LowSupport.h"
#include "HighSupport.h"
#include <vector>
#include <stack>
#include <list>

#define MIN(a, b) (((a) < (b))?(a):(b))
#define MAX(a, b) (((a) > (b))?(a):(b))

const int BATTLE_FRONT = 1;
const int WAY_TO_BATTLE = 2;
const int BACK_FIELD = 3;
const int ROOT = 4;

struct Chamber{
	int chamberId;
	Position rootPos;
	int numRed;
	int numBlack;
	int ChamberType;

	int chamberParent;
	vector<int> children;
};

void dfsChamber(vector<Chamber *> & chamberList, int chamberId, int * board, const Position & pos, int label[][MAP_SIZE], int lowReach[][MAP_SIZE], list<Position> & restoreList, int chamber[][MAP_SIZE]){
	Chamber * thisChamber = chamberList[chamberId];
	thisChamber->numRed = thisChamber->numBlack = 0;
	thisChamber->rootPos = pos;
	thisChamber->ChamberType = BACK_FIELD;

	board[CONVERT_COORD(pos.x, pos.y)] = BLOCK_OCCUPIED;
	restoreList.push_back(pos);

	stack<Position> dfsStack;
	dfsStack.push(pos);

	while (! dfsStack.empty()){
		Position curPos = dfsStack.top();
		dfsStack.pop();
		chamber[curPos.x][curPos.y] = chamberId;
		if (isPosRed(curPos)){
			++thisChamber->numRed;
		}
		else {
			++thisChamber->numBlack;
		}
		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(curPos, direction);
			if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
				if (lowReach[newPos.x][newPos.y] >= label[curPos.x][curPos.y]){
					Chamber * newChamber = new Chamber();
					newChamber->rootPos = newPos;
					newChamber->chamberId = (int)chamberList.size();
					chamberList.push_back(newChamber);
					thisChamber->children.push_back(newChamber->chamberId);
					dfsChamber(chamberList, newChamber->chamberId, board, newPos, label, lowReach, restoreList, chamber);
				}
				else {
					board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
					restoreList.push_back(newPos);
					dfsStack.push(newPos);
				}
			}
		}
	}
}

void buildChamberTree(int * board, const Position & rootPos, vector<Chamber *> & chamberList, int chamber[][MAP_SIZE]){
	int myID = board[CONVERT_COORD(rootPos.x, rootPos.y)];
	board[CONVERT_COORD(rootPos.x, rootPos.y)] = BLOCK_EMPTY;

	int label[MAP_SIZE][MAP_SIZE], cutVertices[MAP_SIZE][MAP_SIZE], lowReach[MAP_SIZE][MAP_SIZE];
	findCutVertices(board, rootPos, label, lowReach, cutVertices);

	list<Position> restoreList;
	Chamber * chamberRoot = new Chamber();
	chamberRoot->rootPos = rootPos;
	chamberRoot->chamberId = (int)chamberList.size();
	chamberList.push_back(chamberRoot);
	dfsChamber(chamberList, chamberRoot->chamberId , board, rootPos, label, lowReach, restoreList, chamber);
	restoreBoard(board, restoreList);
	board[CONVERT_COORD(rootPos.x, rootPos.y)] = myID;
}

void emptyChamberList(vector<Chamber *> & chamberList){
	vector<Chamber *>::iterator itChamberList;
	for (itChamberList = chamberList.begin(); itChamberList != chamberList.end(); ++itChamberList){
		if (*itChamberList != NULL){
			delete (*itChamberList);
			*itChamberList = NULL;
		}
	}
}