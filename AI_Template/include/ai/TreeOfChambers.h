#pragma once

#include "LowSupport.h"
#include "HighSupport.h"
#include <vector>
#include <stack>
#include <list>
#include <map>

#define MIN(a, b) (((a) < (b))?(a):(b))
#define MAX(a, b) (((a) > (b))?(a):(b))

const int BATTLE_FRONT = 1;
const int WAY_TO_BATTLE = 2;
const int BACK_FIELD = 3;

struct Chamber{
	int chamberId;
	Position rootPos;
	int chamberType;
	int maxSelfArea;
	int selfArea;

	map<int, int> enemies;
};

void dfsChamber(vector<Chamber *> & chamberList, int chamberId, int * board, int label[][MAP_SIZE], int lowReach[][MAP_SIZE], int cutVertices[][MAP_SIZE], int chamber[][MAP_SIZE], list<Position> & restoreList){
	Chamber * thisChamber = chamberList[chamberId];

	int numRed = 0, numBlack = 0;
	thisChamber->chamberType = BACK_FIELD;

	int rootPosInitType = board[CONVERT_COORD(thisChamber->rootPos.x, thisChamber->rootPos.y)];
	board[CONVERT_COORD(thisChamber->rootPos.x, thisChamber->rootPos.y)] = BLOCK_OCCUPIED;
	restoreList.push_back(thisChamber->rootPos);

	stack<Position> dfsStack;
	dfsStack.push(thisChamber->rootPos);

	vector<Chamber *> children;

	while (!dfsStack.empty()){
		Position curPos = dfsStack.top();
		dfsStack.pop();
		chamber[curPos.x][curPos.y] = chamberId;
		if (isPosRed(curPos)){
			++numRed;
		}
		else {
			++numBlack;
		}

		for (int direction = 1; direction <= 4; ++direction){
			Position newPos = moveDirection(curPos, direction);
			if (inMatrix(newPos)){
				if (board[CONVERT_COORD(newPos.x, newPos.y)] == rootPosInitType){
					if (cutVertices[curPos.x][curPos.y] == CUT_VERTICE && lowReach[newPos.x][newPos.y] >= label[curPos.x][curPos.y]){
						Chamber * newChamber = new Chamber();
						newChamber->chamberId = (int)chamberList.size();
						newChamber->rootPos = newPos;
						chamberList.push_back(newChamber);
						children.push_back(newChamber);
						dfsChamber(chamberList, newChamber->chamberId, board, label, lowReach, cutVertices, chamber, restoreList);
						if ((newChamber->chamberType == BATTLE_FRONT || newChamber->chamberType == WAY_TO_BATTLE) && thisChamber->chamberType != BATTLE_FRONT){
							thisChamber->chamberType = WAY_TO_BATTLE;
						}
					}
					else {
						board[CONVERT_COORD(newPos.x, newPos.y)] = BLOCK_OCCUPIED;
						restoreList.push_back(newPos);
						dfsStack.push(newPos);
					}
				}
				else if (board[CONVERT_COORD(newPos.x, newPos.y)] != BLOCK_OBSTACLE && board[CONVERT_COORD(newPos.x, newPos.y)] != BLOCK_OCCUPIED){
					thisChamber->chamberType = BATTLE_FRONT;
					if (chamber[newPos.x][newPos.y] != -1){
						if (thisChamber->enemies.find(chamber[newPos.x][newPos.y]) == thisChamber->enemies.end()){
							thisChamber->enemies[chamber[newPos.x][newPos.y]] = 1;
						}
						else {
							++thisChamber->enemies[chamber[newPos.x][newPos.y]];
						}
					}
				}
			}
		}
	}

	thisChamber->maxSelfArea = maxAreaBasedOnRedBlack(numRed, numBlack, isPosRed(thisChamber->rootPos));
	thisChamber->selfArea = maxAreaBasedOnRedBlack(numRed, numBlack, isPosRed(thisChamber->rootPos));
	for (int idChild = 0; idChild < (int)children.size(); ++idChild){
		Chamber * nextChamber = children[idChild];
		if (nextChamber->chamberType == BACK_FIELD){
			int areaPlusChild = maxAreaBasedOnRedBlack(numRed, numBlack, isPosRed(thisChamber->rootPos),
				!isPosRed(nextChamber->rootPos)) + nextChamber->maxSelfArea;
			thisChamber->maxSelfArea = MAX(thisChamber->maxSelfArea, areaPlusChild);
		}
	}

	if (thisChamber->chamberType == BATTLE_FRONT && thisChamber->enemies.size() != 0){
		map<int, int>::iterator itEnemies;
		for (itEnemies = thisChamber->enemies.begin(); itEnemies != thisChamber->enemies.end(); ++itEnemies){
			Chamber * enemyChamber = chamberList[itEnemies->first];
			enemyChamber->enemies.insert(pair<int, int>(chamberId, itEnemies->second));
		}
	}
}

void buildChamberTree(int * board, int * vonoroiBoard, const Position & rootPos, vector<Chamber *> & chamberList, int chamber[][MAP_SIZE]){
	int myID = board[CONVERT_COORD(rootPos.x, rootPos.y)];
	
	board[CONVERT_COORD(rootPos.x, rootPos.y)] = BLOCK_EMPTY;
	int label[MAP_SIZE][MAP_SIZE], cutVertices[MAP_SIZE][MAP_SIZE], lowReach[MAP_SIZE][MAP_SIZE];
	findCutVertices(board, rootPos, label, lowReach, cutVertices);
	board[CONVERT_COORD(rootPos.x, rootPos.y)] = myID;

	list<Position> restoreList;

	/*for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			cout << cutVertices[idRow][idCol] << " ";
		}
		cout << endl;
	}
	cout << endl;*/

	int myVonoroiId = vonoroiBoard[CONVERT_COORD(rootPos.x, rootPos.y)];

	Chamber * chamberRoot = new Chamber();
	chamberRoot->rootPos = rootPos;
	chamberRoot->chamberId = (int)chamberList.size();
	chamberRoot->rootPos = rootPos;
	chamberList.push_back(chamberRoot);
	dfsChamber(chamberList, chamberRoot->chamberId, vonoroiBoard, label, lowReach, cutVertices, chamber, restoreList);

	list<Position>::iterator itRestoreList;
	for (itRestoreList = restoreList.begin(); itRestoreList != restoreList.end(); ++itRestoreList){
		vonoroiBoard[CONVERT_COORD(itRestoreList->x, itRestoreList->y)] = myVonoroiId;
	}
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