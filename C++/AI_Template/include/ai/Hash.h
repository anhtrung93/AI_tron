#pragma once

#include <ai/AI.h>
#include <string>
#include <ctime>

#define NUM_ZOBRIST_TYPE 3
#define NUM_ZOBRIST_PLAYER 2

using namespace std;

//Zobrist Hashing method

unsigned long long zobristTable[MAP_SIZE * MAP_SIZE][NUM_ZOBRIST_TYPE];
unsigned long long zobristTurn[NUM_ZOBRIST_PLAYER];

const int ZOBRIST_ME = 0;
const int ZOBRIST_ENEMY = 1;
const int ZOBRIST_OBSTACLE = 2;

const int ZOBRIST_MY_TURN = 0;
const int ZOBRIST_ENEMY_TURN = 1;

unsigned int rand256() {
	static unsigned int const LIMIT_RAND = RAND_MAX - RAND_MAX % 256;
	unsigned result = rand();
	while (result >= LIMIT_RAND) {
		result = rand();
	}
	return result % 256;
}

unsigned long long uint64Rand() {
	unsigned long long result = 0ULL;
	for (int count = 8; count > 0; --count) {
		result = 256U * result + rand256();
	}
	return result;
}

void initZobrist(){
	srand((unsigned int) time(NULL));
	for (int pos = 0; pos < MAP_SIZE * MAP_SIZE; ++pos){
		for (int type = 0; type < NUM_ZOBRIST_TYPE; ++type){
			zobristTable[pos][type] = uint64Rand();
		}
	}
	zobristTurn[ZOBRIST_MY_TURN] = uint64Rand();
	zobristTurn[ZOBRIST_ENEMY_TURN] = uint64Rand();
}

unsigned long long hashBoard(int * board, const Position & myPos, const Position & enemyPos, bool isMyTurn){
	unsigned long long result = 0;
	for (int idRow = 0; idRow < MAP_SIZE; ++idRow){
		for (int idCol = 0; idCol < MAP_SIZE; ++idCol){
			int id = CONVERT_COORD(idRow, idCol);
			if (id == CONVERT_COORD(myPos.x, myPos.y)){
				result ^= zobristTable[id][ZOBRIST_ME];
			}
			else if (id == CONVERT_COORD(enemyPos.x, enemyPos.y)){
				result ^= zobristTable[id][ZOBRIST_ENEMY];
			}
			else if (board[id] != BLOCK_EMPTY){
				result ^= zobristTable[id][ZOBRIST_OBSTACLE];
			}
		}
	}
	if (isMyTurn){
		result ^= zobristTurn[ZOBRIST_MY_TURN];
	}
	else {
		result ^= zobristTurn[ZOBRIST_ENEMY_TURN];
	}
	return result;
}

unsigned long long hashMove(unsigned long long oldHash, const Position & fromPos, const Position & toPos, bool isMyTurn){
	if (isMyTurn){
		oldHash ^= zobristTable[CONVERT_COORD(fromPos.x, fromPos.y)][ZOBRIST_ME];
		oldHash ^= zobristTable[CONVERT_COORD(toPos.x, toPos.y)][ZOBRIST_ME];
	}
	else {
		oldHash ^= zobristTable[CONVERT_COORD(fromPos.x, fromPos.y)][ZOBRIST_ENEMY];
		oldHash ^= zobristTable[CONVERT_COORD(toPos.x, toPos.y)][ZOBRIST_ENEMY];
	}
	oldHash ^= zobristTable[CONVERT_COORD(fromPos.x, fromPos.y)][ZOBRIST_OBSTACLE];
	oldHash ^= zobristTurn[ZOBRIST_MY_TURN];
	oldHash ^= zobristTurn[ZOBRIST_ENEMY_TURN];
	return oldHash;
}