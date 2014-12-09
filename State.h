#ifndef STATE_H
#define STATE_H

#include <list>
#include "Block.h"

class State {

public:
	std::list<Block> blockList;
	std::string holding;

};


#endif