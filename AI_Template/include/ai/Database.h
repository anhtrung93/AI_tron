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
	StateInfo(bool initIsSplit, int initDepthExplore, int initEstVal, bool initNoMoreLeaves) : isSplit(initIsSplit), depthExplore(initDepthExplore), estVal(initEstVal){
		for (int id = 0; id < 5; ++id){
			nextStates[id] = NULL;
		}
	}
};

map<unsigned long long, StateInfo> database[MAX_TOTAL_UNSPLIT_DEPTH + 1];

StateInfo * insertState(int totalDepth, unsigned long long hashVal, const StateInfo & stateInfo){
	pair<map<unsigned long long, StateInfo>::iterator, bool> insertResult;
	insertResult = database[totalDepth].insert(pair<unsigned long long, StateInfo>(hashVal, stateInfo));
	if (insertResult.second == false){
		insertResult.first->second = stateInfo;
	}
	return &(insertResult.first->second);
}

StateInfo * getStateInfo(int totalDepth, unsigned long long hashVal){
	map<unsigned long long, StateInfo>::iterator itFind = database[totalDepth].find(hashVal);
	if (itFind == database[totalDepth].end()){
		return insertState(totalDepth, hashVal, StateInfo());
	}
	else {
		return &(itFind->second);
	}
}

void removeDatabaseAtDepth(int totalDepth){
	if (totalDepth >= 0 && totalDepth < MAX_TOTAL_UNSPLIT_DEPTH){
		//cout << "remove at " << totalDepth << " with size = " << database[totalDepth].size() << endl;
		database[totalDepth].clear();
	}
}

/*struct TrieNode {
	StateInfo * info;
	TrieNode * children;

	TrieNode() : info(NULL), children(NULL) {}
	~TrieNode(){
	if (info != NULL){
	delete info;
	info = NULL;
	}
	if (children != NULL){
	delete[] children;
	children = NULL;
	}
	}

	StateInfo * getState(unsigned long long hashVal){
	if (hashVal == 0){
	if (this->info == NULL){
	this->info = new StateInfo();
	}
	return this->info;
	}
	int childNum = hashVal & 0xFF;//hashVal % 256
	if (children == NULL){
	children = new TrieNode[256];
	}
	return children[childNum].getState(hashVal >> 8);//hashVal / 256
	}

	StateInfo * insertState(unsigned long long hashVal, const StateInfo & info){
	StateInfo * tmp = getState(hashVal);
	*tmp = info;
	return tmp;
	}
	};

	TrieNode database;

	StateInfo * getStateInfo(unsigned long long hashVal){
	return database.getState(hashVal);
	}*/