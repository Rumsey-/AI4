#ifndef RULE_H
#define RULE_H

#include <string>
#include <iostream>
#include <list>
#include "Block.h"
#include "State.h"

class Rule {

public:
	std::string currentBlock;
	std::string onBlock; //if the block is being put down, this specifies where
	
	bool pickUpFlag; //true if the block is being picked up
	void applyRule(State &currentState);

	void undoRule(State &currentState);

	void printRule();
};

void Rule::applyRule(State &currentState) {
	//pick up block
	std::list<Block>::iterator blockIter;
	//currentState.currentBestPath.push_back(this); //current rule added to the path of rules

	if (pickUpFlag==true) {
		currentState.holding = currentBlock->name;
		for(blockIter=currentState.blockList.begin();blockIter!=currentState.blockList.end(); blockIter++) {
		(*blockIter)->held = true;
		(*blockIter)->onTable = false;
		(*blockIter)->clear = false;
		(*blockIter)->isOn = "HELD";
		}
		if (currentBlock->isOn.compare("TABLE")!=0)
		{
			for(blockIter=currentState.blockList.begin();blockIter!=currentState.blockList.end(); blockIter++) {
				if(currentBlock->isOn.compare((*blockIter)->name)==0) (*blockIter)->clear=true;
		}
		currentState.holding="HE";

	}
	//if putting on table
	else if (!onBlock.compare("TABLE")) {
		for(blockIter=currentState.blockList.begin();blockIter!=currentState.blockList.end(); blockIter++) {
		(*blockIter)->held = false;
		(*blockIter)->onTable = true;
		currentState.holding = "HE";
		}
	}

	else {
		for(blockIter=currentState.blockList.begin();blockIter!=currentState.blockList.end(); blockIter++) {
			(*blockIter)->clear=true;
			(*blockIter)->held=false;
			(*blockIter)->isOn=onBlock;
		}
		for(blockIter=currentState.blockList.begin();blockIter!=currentState.blockList.end(); blockIter++) {
			(*blockIter)->clear=false;
		}
		currentState.holding="NONE";
	}

}

void Rule::undoRule(State &currentState) {
	//put block back where it came from
	if (pickUpFlag == true) {
		currentBlock->held = false; 
		//if it came from the table
		if (onBlock == NULL) {
			currentBlock->onTable = true;
			currentBlock->clear = true;
		}
		else {
			currentBlock->isOn = onBlock;
		}
	}
	//if it wasn't picked up, it was put down, so pick it up to undo
	else {
		if (onBlock!=NULL) onBlock->clear = true;
		for(blockIter=currentState.blockList.begin();blockIter!=currentState.blockList.end(); blockIter++) {
			(*blockIter)->held = true;
			(*blockIter)->isOn = "NONE";
			currentState.holding = currentBlock;
	}

}

void Rule::printRule() {

	if(pickUpFlag) {
		std::cout << "PICKUP(" << currentBlock->name << ")\t:\t";
		return;
	}
	else if(onBlock==NULL) {
		std::cout << "PUTDOWN(" << currentBlock->name << ")\t:\t";
	}

	else {
		std::cout << "STACK(" << currentBlock->name << ", " << onBlock->name << ")\t:\t";
	}

}

#endif