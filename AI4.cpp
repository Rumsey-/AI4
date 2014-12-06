
	
#include<string>
#include<iostream>
#include<list>
#include<vector>
#include<ios>
#include<fstream>
#include<sstream>
#include<iterator>

#include "Block.h"
#include "Rule.h"
#include "State.h"

using namespace std;

//check if in the goal state
/*bool checkGoalState(State &currentState) {
	list<Block*>::iterator blockIter;
	for (blockIter = currentState.blockList.begin(); blockIter != currentState.blockList.end(); blockIter++) {
		if ((*blockIter)->clear != (*blockIter)->clearGoal) return false;
		if ((*blockIter)->onTable != (*blockIter)->onTableGoal) return false;
		if ((*blockIter)->isOn != (*blockIter)->isOnGoal) return false;
	}
	if (currentState.holding != currentState.holdingGoal) return false;
	else return true;
}*/
//returns true if the states are identical

bool compareStates (State &state1, State &state2) {
	list<Block>::iterator blockIter1;
	list<Block>::iterator blockIter2;

	blockIter2=state2.blockList.begin();
	for (blockIter1=state1.blockList.begin(); blockIter1!=state1.blockList.end(); blockIter1++) {
		if ((*blockIter1).clear!=(*blockIter2).clear) return false;
		if ((*blockIter1).held!=(*blockIter2).held) return false;
		if ((*blockIter1).onTable!=(*blockIter2).onTable) return false;
		if ((*blockIter1).isOn==NULL) {
			if((*blockIter2).isOn!=NULL) return false;
		}
		else if((*blockIter2).isOn==NULL) {
			if((*blockIter1).isOn!=NULL) return false;
		}
		else {
			if((*blockIter1).isOn->name.compare((*blockIter2).isOn->name)) return false;
		}
		blockIter2++;
	}
	if(state1.holding==NULL) {
		if(state2.holding!=NULL) return false;
	}
	else if(state1.holding->name.compare(state2.holding->name)) return false;
	return true;

}

void printState(State &currentState) {

	list<Block>::iterator blockIter;

	for (blockIter = currentState.blockList.begin(); blockIter != currentState.blockList.end(); blockIter++) {

		if ((*blockIter).onTable) {
			cout << "ON_TABLE(" << (*blockIter).name << "), ";
		}

		if ((*blockIter).isOn != NULL) {
			cout << "ON(" << (*blockIter).name << ", " << (*blockIter).isOn->name << "), ";
		}

		if ((*blockIter).clear) {
			cout << "CLEAR(" << (*blockIter).name << "), ";
		}
	}

	if (currentState.holding == NULL) {
		cout << "HE" << endl;
	}
	else {
		cout << "HOLDING(" << currentState.holding->name << ")" << endl;
	}
}


void backTrack(Rule* currentRule, State &currentState, State &goalState, int &globalMin, list<Rule*> &currentPath, list<Rule*> &bestPath, list<State> &statePath) {

	list<Rule*> rulesList; //the rules which apply from this node
	Rule* ruleCurrent;
	int currentMin = 0;
	currentRule->applyRule(currentState);
	
	list<State>::iterator stateIter;
	currentRule->printRule();
	printState(currentState);

	currentPath.push_back(currentRule);
	

	for (stateIter=statePath.begin(); stateIter!=statePath.end(); stateIter++) {
		if (compareStates(currentState, (*stateIter))) {
			statePath.push_back(currentState);
			return;
		}
	}

	statePath.push_back(currentState);
	


	//if applying the current rule puts us over the current minimum, 
	//there is no need to explore further nodes from this branch 

	if (currentPath.size() > globalMin) {
		return;
	} 
	//however, if it is less than the global and we're in the goal, a new best is found
	if (compareStates(currentState,goalState)) {
		globalMin = currentPath.size();
		bestPath = currentPath;
		return;
	}

	//check the current state against all other states that the current application of rules has resulted in
	//this is to prevent the duplication or looping of states

	

	list<Block>::iterator blockIter;
	//find the applicable rules

	if (currentState.holding != NULL) { //if a block is being held, only rules where the block is put down apply
		for (blockIter = currentState.blockList.begin(); blockIter != currentState.blockList.end(); blockIter++) {
			//if the block is clear the held block can be put down here
			if ((*blockIter).clear) {
				Rule* newRule = new Rule;
				newRule->currentBlock = currentState.holding;
				newRule->onBlock = &(*blockIter);
				newRule->pickUpFlag = false;//putting down not picking up
				rulesList.push_back(newRule);
			}
		}
		//the block could always also be put on the table
		Rule* newRule = new Rule;
		newRule->currentBlock = currentState.holding;
		newRule->onBlock = NULL; //not putting it on a block but the table
		newRule->pickUpFlag = false; //putting down
		rulesList.push_back(newRule);

	}
	//otherwise, pick up a block
	else {
		for (blockIter = currentState.blockList.begin(); blockIter != currentState.blockList.end(); blockIter++) {
			//if the block is clear it can be picked up
			if ((*blockIter).clear) {
				(*blockIter).clear = false; //no longer clear, since it's picked up
				Rule* newRule = new Rule;
				newRule->currentBlock = &(*blockIter); //picking up the clear block that is found
				newRule->onBlock = NULL;
				newRule->pickUpFlag = true;//putting down not picking up
				rulesList.push_back(newRule);
			}
		}
	}
	
	

	

	//while there are rules to apply to this node, it does them and begins recursion
	while (rulesList.size() != 0) {
		ruleCurrent = rulesList.front();
		rulesList.pop_front();
		backTrack(ruleCurrent, currentState, goalState, globalMin, currentPath, bestPath, statePath);
		int foo = currentPath.size();
		currentPath.pop_back();
		foo = statePath.size();
		statePath.pop_back();
		ruleCurrent->undoRule(currentState);
	}

	return;

}
//a starting point for recursion, checks available rules from starting point of position
//assumes that starting point is not a goal state.
void backTrackStart(State &currentState, State &goalState, int &globalMin, list<Rule*> &currentPath, list<Rule*> &bestPath) {

	list<Rule*> rulesList; //the rules which apply from this node
	Rule* ruleCurrent;
	
	//if applying the current rule puts us over the current minimum, 
	//there is no need to explore further nodes from this branch 

	if (currentPath.size() > globalMin) {
		currentPath.pop_back();
		return;
	}
	//however, if it is less than the global and we're in the goal, a new best is found
	if (compareStates(currentState,goalState)) {
		globalMin = currentPath.size();
		bestPath = currentPath;
		return;
	}
	list<Block>::iterator blockIter;
	//find the applicable rules

	if (currentState.holding != NULL) { //if a block is being held, only rules where the block is put down apply
		for (blockIter = currentState.blockList.begin(); blockIter != currentState.blockList.end(); blockIter++) {
			//if the block is clear the held block can be put down here
			if ((*blockIter).clear) {
				(*blockIter).clear = false; //no longer clear, has a block on it
				Rule* newRule = new Rule;
				newRule->currentBlock = currentState.holding;
				newRule->onBlock = &(*blockIter);
				newRule->pickUpFlag = false;//putting down not picking up
				rulesList.push_back(newRule);
			}
		}
		//the block could always also be put on the table
		Rule* newRule = new Rule;
		newRule->currentBlock = currentState.holding;
		newRule->onBlock = NULL; //not putting it on a block but the table
		newRule->pickUpFlag = false; //putting down
		rulesList.push_back(newRule);

	}
	//otherwise, pick up a block
	else {
		for (blockIter = currentState.blockList.begin(); blockIter != currentState.blockList.end(); blockIter++) {
			//if the block is clear it can be picked up
			if ((*blockIter).clear) {
				Rule* newRule = new Rule;
				newRule->currentBlock = &(*blockIter); //picking up the clear block that is found
				newRule->onBlock = NULL;
				newRule->pickUpFlag = true;//putting down not picking up
				rulesList.push_back(newRule);
			}
		}
	}


	list<State> statePath;
	statePath.push_back(currentState);


	//while there are rules to apply to this node, it does them and begins recursion
	while (rulesList.size() != 0) {
		ruleCurrent = rulesList.front();
		rulesList.pop_front();
		backTrack(ruleCurrent, currentState, goalState, globalMin, currentPath, bestPath, statePath);
		int foo = currentPath.size();
		currentPath.pop_back();
		ruleCurrent->undoRule(currentState);
	}
	return;

}

//printing the goal state for debug purposes




int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "Usage: hw4 input.txt" << endl;
		return -1;
	}

	ifstream inputFile(argv[1]);
	//check valid inputfile
	if (!inputFile.is_open()) {
		cout << "Unable to open file" << endl;
		return -1;
	}

	string inputBuffer;
	string buffer;
	string tempName;

	list<Block> blockList;
	list<Block>::iterator blockIter;

	State startState;
	startState.holding=NULL;
	State goalState;
	goalState.holding=NULL;

	int i, j;

	while (getline(inputFile, inputBuffer)) {
		buffer.clear();
		for (i = 0; i < inputBuffer.length(); i++) {
			if (inputBuffer[i] == ':') break;
			else buffer.push_back(inputBuffer[i]);
		}
		if (!buffer.compare("BLOCKS")) {
			getline(inputFile, inputBuffer);

			istringstream iss(inputBuffer); //creating a stringstream from the gotten line

			vector<string> tokens;
			copy(istream_iterator<string>(iss), //tokenize the gotten line using istream_iterator
				istream_iterator<string>(),
				back_inserter(tokens));

			//build block list here
			for (i = 0; i < tokens.size(); i++) {
				buffer = tokens[i];
				tempName.clear();
				for (j = 0; j < buffer.length (); j++) {
					if (buffer[j] == ',') break;
					else  tempName.push_back(buffer[j]);
					
				}
				Block newBlock;
				newBlock.name = tempName;
				blockList.push_back(newBlock);

			}
		}

		//build initial state
		if (!buffer.compare("INITIAL STATE")) {
			while (1) {
				getline(inputFile, inputBuffer);
				if (inputBuffer.size() == 0) break;
				buffer.clear();
				for (i = 0; i < inputBuffer.length(); i++) {
					if (inputBuffer[i] == '(')
					{
						tempName.clear();
						while (i < inputBuffer.length()) {
							i++;
							if (inputBuffer[i] == ')') break;
							else  tempName.push_back(inputBuffer[i]);

						}
					}
					else buffer.push_back(inputBuffer[i]);
				}
				if (!buffer.compare("ON_TABLE")) {
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							(*blockIter).onTable = true;
						}
					}
				}
				if (!buffer.compare("CLEAR")) {
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							(*blockIter).clear = true;
						}
					}
				}
				if (!buffer.compare("ON")) {
					string tempName2;
					for (i = 0; i < tempName.length(); i++) {
						if (tempName.front() == ',') {
							tempName.erase(0, 1);
							break;
						}
						else {
							
							tempName2.push_back(tempName.front());
							tempName.erase(0, 1);
						}

					}
					Block* tempBlock = NULL;
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							tempBlock = &(*blockIter);
						}
					}

					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName2)) {
							(*blockIter).isOn = tempBlock;
						}
					}
				}

				if (!buffer.compare("HE")) {
					startState.holding = NULL;
				}

				if (!buffer.compare("HOLDING")) {
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							(*blockIter).held = true;
							startState.holding = &(*blockIter);
						}
					}
				}
			}
			startState.blockList = blockList;
		}
		for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
			(*blockIter).clear = false;
			(*blockIter).held = false;
			(*blockIter).onTable = false;
			(*blockIter).isOn = NULL;
		}
		//build goal state
		if (!buffer.compare("GOAL STATE")) {
			while (1) {
				getline(inputFile, inputBuffer);
				if (inputBuffer.size() == 0) break;
				buffer.clear();
				for (i = 0; i < inputBuffer.length(); i++) {
					if (inputBuffer[i] == '(')
					{
						tempName.clear();
						while (i < inputBuffer.length()) {
							i++;
							if (inputBuffer[i] == ')') break;
							else  tempName.push_back(inputBuffer[i]);

						}
					}
					else buffer.push_back(inputBuffer[i]);
				}
				if (!buffer.compare("ON_TABLE")) {
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							(*blockIter).onTable = true;
						}
					}
				}
				if (!buffer.compare("CLEAR")) {
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							(*blockIter).clear = true;
						}
					}
				}
				if (!buffer.compare("ON")) {
					string tempName2;
					for (i = 0; i < tempName.length(); i++) {
						if (tempName.front() == ',') {
							tempName.erase(0, 1);
							break;
						}
						else {

							tempName2.push_back(tempName.front());
							tempName.erase(0, 1);
						}

					}
					Block* tempBlock = NULL;
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							tempBlock = &(*blockIter);
							tempBlock->clear = false;
						}
					}

					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName2)) {
							(*blockIter).isOn = tempBlock;
						}
					}
				}

				if (!buffer.compare("HE")) {
					goalState.holding = NULL;
				}

				if (!buffer.compare("HOLDING")) {
					for (blockIter = blockList.begin(); blockIter != blockList.end(); blockIter++) {
						if (!(*blockIter).name.compare(tempName)) {
							(*blockIter).held = true;
							goalState.holding = &(*blockIter);
						}
					}
				}
			}
			goalState.blockList = blockList;
		}

	}

		int min = 1000; //making no assumptings about the minimum s
	list<Rule*> currentPath;
	list<Rule*> bestPath;
	printState(startState);
	printState(goalState);

	State currentState = startState;

	backTrackStart(currentState, goalState, min, currentPath, bestPath);
	
	bool test = compareStates(startState, goalState);

	//print path: iterate, print rule, apply rule, print state

	Rule newRule;
	newRule.pickUpFlag=true;
	newRule.currentBlock=&(startState.blockList.front());

	newRule.printRule();
	newRule.applyRule(startState);
	printState(startState);
	test = compareStates(currentState,startState);
	
	return 0;

}

