#pragma once

#include <ai/AI.h>
#include <ai/Hash.h>
#include <ai/LowSupport.h>
#include <map>
#include <mutex>


#define uint64 unsigned long long
#define MAX_TOTAL_UNSPLIT_DEPTH 110

struct StateInfo{
public:
	bool isSplit;	
	int depthExplore; //-1 if has not been explored before	
	int depthFromChildren;
	int estVal;

	StateInfo * nextStates[5];

	StateInfo() : isSplit(false), depthExplore(-1), depthFromChildren(-1), estVal(0){
		for (int id = 0; id < 5; ++id){
			this->nextStates[id] = NULL;
		}
	}

	void setValues(int newEstVal, int newDepthExplore, int newDepthChildren, bool newIsSplit){
		this->estVal = newEstVal;
		this->depthExplore = newDepthExplore;
		this->depthFromChildren = newDepthChildren;
		this->isSplit = newIsSplit;
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
		database[totalDepth].clear();
	}
}