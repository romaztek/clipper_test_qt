#ifndef GHOST_H
#define GHOST_H

#include <iostream>

class Ghost
{
public:
    int getCollisionCount();
	void setCollisionCount(int newCount);
	int getType();
	void setType(int newType);
	int getState();
	void setState(int newState);
		
private:
	int type = 0;
	int collisionCount = 0;
	int state = 0;  // -1: death | 0: live
	
};

#endif
