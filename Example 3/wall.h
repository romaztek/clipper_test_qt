#ifndef WALL_H
#define WALL_H

#include <iostream>

#include <Box2D/Box2D.h>
#include "ghost.h"

extern const int SCALE;

class Wall : public Ghost
{
	public:
		Wall(float _x, float _y, float _width, float _height, int _type, b2World *_world);
		~Wall();
		
		float x, y, width, height;
		b2World *world;
		b2Body *body;
		
		
};

#endif
