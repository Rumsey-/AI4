#ifndef RULE_H
#define RULE_H

#include <iostream>
#include <list>
#include "Block.h"
#include "State.h"

class Rule {

public:
	Block* currentBlock;
	Block* onBlock; //if the block is being put down, this specifies where
	
	bool pickUpFlag; //true if the block is being picked up
	void applyRule(State &currentState);

	void undoRule(State &currentState);

	void printRule();
};

void Rule::applyRule(State &currentState) {
	//pick up block
	std::list<Block*>::iterator blockIter;
	//currentState.currentBestPath.push_back(this); //current rule added to the path of rules

	if (pickUpFlag==true) {
		currentState.holding = currentBlock;
		currentBlock->held = true;
		currentBlock->onTable = false;
		currentBlock->clear = false;
		if (currentBlock->isOn!=NULL) currentBlock->isOn->clear = true;
		currentBlock->isOn = NULL;

	}
	//if putting on table
	else if (onBlock==NULL) {
		currentState.holding->held = false;
		currentState.holding->onTable = true;
		currentState.holding = NULL;
	}

	else {
		onBlock->clear = false;
		currentState.holding->isOn = onBlock;
		currentState.holding->clear = true;
		currentState.holding = NULL;
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
		currentBlock->held = true;
		currentBlock->isOn = NULL;
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