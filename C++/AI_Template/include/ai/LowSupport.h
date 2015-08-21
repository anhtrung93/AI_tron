#pragma once

#include <ai/AI.h>
#include <list>
#include <Windows.h>
#include <mutex>

//#define UNIT_TEST
//#define PRINT_LOG
//#define PRINT_LOG2
//#define NOT_STEAL
//#define PRINT_TEST
#define COPY_SPECIAL

#define MIN(a, b) (((a) < (b))?(a):(b))
#define MAX(a, b) (((a) > (b))?(a):(b))

using namespace std;

const int BLOCK_OCCUPIED = 6;
const int UNKNOWN_DIRECTION = 0;
const int MY_VONOROI_EMPTY = 7;
const int ENEMY_VONOROI_EMPTY = 8;
const int MY_BLOCK_OCCUPIED = 7;
const int ENEMY_BLOCK_OCCUPIED = 8;

const int NORMAL_VERTICE = 0;
const int CUT_VERTICE = 1;

const int MOVE_X[] = { 0, -1, 0, 1, 0 };
const int MOVE_Y[] = { 0, 0, -1, 0, 1 };

const int INF = 400;
const int START_DEPTH_LEVEL = 12;
const int MAX_TIME = 2600;
const int UNKNOWN_HEUR_VAL = 0;

const int STOP_SEARCH_VAL = 210;

bool timeOut = false;
int totalDepth = -1;
bool isSplitStatus = false;
mutex mtx;

bool isSecCopyFirst = true;
bool isMeFirst;

inline bool isSpecialBoard(int * board){
	if (board[CONVERT_COORD(4, 5)] == BLOCK_OBSTACLE || board[CONVERT_COORD(5, 4)] == BLOCK_OBSTACLE){
		return true;
	}
	return false;
}

inline bool isPalinBoard(int * board, const Position & myPos, const Position & enemyPos){
	if (board[CONVERT_COORD(5, 5)] != BLOCK_EMPTY){
		return false;
	}
	else if (myPos.x != 10 - enemyPos.x || myPos.y != 10 - enemyPos.y){
		return false;
	}
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol <= MAP_SIZE / 2; ++idCol){
			if (board[CONVERT_COORD(idRow, idCol)] == BLOCK_EMPTY){
				if (board[CONVERT_COORD(10 - idRow, 10 - idCol)] != BLOCK_EMPTY){
					return false;
				}
			}
			else if (board[CONVERT_COORD(10 - idRow, 10 - idCol)] == BLOCK_EMPTY){
				return false;
			}
		}
	}
	return true;
}

inline int whichDirection(const Position & pos, const Position & nextPos){
	for (int direction = 1; direction <= 4; ++direction){
		if (nextPos.x == pos.x + MOVE_X[direction] && nextPos.y == pos.y + MOVE_Y[direction]){
			return direction;
		}
	}
	return UNKNOWN_DIRECTION;
}

inline bool inMatrix(const Position & pos) {
	if (pos.x >= 0 && pos.x < MAP_SIZE){
		if (pos.y >= 0 && pos.y < MAP_SIZE){
			return true;
		}
	}
	return false;
}

inline void restoreBoard(int * board, const list<Position> & restoreList) {
	for (list<Position>::const_iterator it = restoreList.begin(); it != restoreList.end(); ++it){
		board[CONVERT_COORD(it->x, it->y)] = BLOCK_EMPTY;
	}
}

inline Position moveDirection(const Position & pos, int direction) {
	return Position(pos.x + MOVE_X[direction], pos.y + MOVE_Y[direction]);
}

inline bool isPosRed(const Position & pos){
	return ((pos.x + pos.y) % 2 == 0);
}

inline int maxAreaBasedOnRedBlack(int numRed, int numBlack, bool isRedInit) {
	if (isRedInit && numRed > numBlack){
		return numBlack * 2 + 1;
	}
	else if (isRedInit == false && numBlack > numRed){
		return numRed * 2 + 1;
	}
	else {
		return MIN(numRed, numBlack) * 2;
	}
}

inline int maxAreaBasedOnRedBlack(int numRed, int numBlack, bool isRedInit, bool isRedEnd) {
	if (isRedInit && isRedEnd){
		return MIN(2 * numRed - 1, 2 * numBlack + 1);
	}
	else if (isRedInit == false && isRedEnd == false){
		return MIN(2 * numBlack - 1, 2 * numRed + 1);
	}
	else {
		return MIN(numRed, numBlack) * 2;
	}
}

inline int getConnection(int * board, const Position & pos) {
	int result = 0;
	for (int direction = 1; direction <= 4; ++direction){
		Position newPos = moveDirection(pos, direction);
		if (inMatrix(newPos) && board[CONVERT_COORD(newPos.x, newPos.y)] == BLOCK_EMPTY){
			++result;
		}
	}
	return result;
}

void printBoard(int * board){
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			/*string += "\'";
			if (board[CONVERT_COORD(idRow, idCol)] < 10 && board[CONVERT_COORD(idRow, idCol)] >= 0){
			string += "   ";
			}
			else if (board[CONVERT_COORD(idRow, idCol)] < 100 && board[CONVERT_COORD(idRow, idCol)] > -10){
			string += "  ";
			}
			else if (board[CONVERT_COORD(idRow, idCol)] < 1000 && board[CONVERT_COORD(idRow, idCol)] > -100){
			string += " ";
			}
			string += board[CONVERT_COORD(idRow, idCol)] + "\'";*/
			cout << board[CONVERT_COORD(idRow, idCol)] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

bool comparePairMinSecond(const pair<int, int> & a, const pair<int, int> & b){
	return (a.second < b.second);
}

bool comparePairMaxSecond(const pair<int, int> & a, const pair<int, int> & b){
	return (a.second > b.second);
}

bool comparePairPairMaxSecond(const pair<int, pair<int, int> > & a, const pair<int, pair<int, int> > & b){
	if (a.second.first == b.second.first){
		return (a.second.second > b.second.second);
	}
	else {
		return (a.second.first > b.second.first);
	}
}

bool compareSuperPairMaxSecond(const pair<int, pair<int, unsigned long long> > & a, const pair<int, pair<int, unsigned long long> > & b){
	return (a.second.first > b.second.first);
}

bool compareSuperPairMinSecond(const pair<int, pair<int, unsigned long long> > & a, const pair<int, pair<int, unsigned long long> > & b){
	return (a.second.first < b.second.first);
}

bool compairPosMinSecondSecond(const pair<Position, pair<int, int>> & a, const pair<Position, pair<int, int>> & b){
	return (a.second.second < b.second.second);
}

unsigned long long getCurTime(){
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	* to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	unsigned long long ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

	return ret;
}

//*****************************************************Cut Vertices Support ********************************************************/
struct CutInfo {
	bool isRed;
	int area;

	CutInfo(){}
	CutInfo(bool _isRed, int _area) : isRed(_isRed), area(_area) {}

	CutInfo operator = (const CutInfo & other){
		isRed = other.isRed;
		area = other.area;
		return *this;
	}
};

struct DfsUpperObj{
	Position pos;
	bool isRed;
	int numRed, numBlack;
	list<CutInfo *> nextCuts;

	~DfsUpperObj(){
		for (list<CutInfo *>::iterator it = nextCuts.begin(); it != nextCuts.end(); ++it){
			if (*it != NULL){
				delete *it;
				*it = NULL;
			}
		}
		nextCuts.clear();
	}
};