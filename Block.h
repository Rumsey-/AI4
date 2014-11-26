#ifndef BLOCK_H
#define BLOCK_H

#include<string>

class Block {

public:
	std::string name;

	bool onTable;

	bool clear;

	bool held;

	Block* isOn;
	
};

#endif