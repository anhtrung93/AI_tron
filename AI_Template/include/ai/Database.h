#pragma once

#include <ai/AI.h>
#include <ai/Hash.h>
#include <map>

#define MAX_TOTAL_UNSPLIT_DEPTH 110

struct StateInfo{
	bool isSplit;
	int depthExplore; //-1 if has not been explored before
	int estVal;

	StateInfo * nextStates[5];

	StateInfo() : isSplit(false), depthExplore(-1), estVal(0) {
		for (int id = 0; id < 5; ++id){
			nextStates[id] = NULL;
		}
	}
};

map<unsigned long long, StateInfo> database[MAX_TOTAL_UNSPLIT_DEPTH + 1];

inline StateInfo * getStateInfo(int totalDepth, unsigned long long hashVal) {
	pair<map<unsigned long long, StateInfo>::iterator, bool> insertResult;
	insertResult = database[totalDepth].insert(pair<unsigned long long, StateInfo>(hashVal, StateInfo()));
	return &(insertResult.first->second);
}

void removeDatabaseAtDepth(int totalDepth){
	if (totalDepth >= 0 && totalDepth < MAX_TOTAL_UNSPLIT_DEPTH){
		//cout << "remove at " << totalDepth << " with size = " << database[totalDepth].size() << endl;
		database[totalDepth].clear();
	}
}