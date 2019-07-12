#include "ghost.h"

int Ghost::getCollisionCount() {
	return collisionCount;
}

void Ghost::setCollisionCount(int newCount) {
	if(newCount == 1)
		collisionCount++;
	else
		collisionCount--;
}

int Ghost::getType() {
	return type;
}

void Ghost::setType(int newType) {
	type = newType;
}

int Ghost::getState() {
	return state;
}

void Ghost::setState(int newState) {
	state = newState;
}
