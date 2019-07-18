#include "wall.h"

Wall::Wall(float _x, float _y, float _width, float _height, int _type, b2World *_world) :
	x(_x),
	y(_y),
	width(_width),
	height(_height),
	world(_world)
{
	this->setType(_type);
	b2BodyDef groundBodyDef;
	groundBodyDef.position = b2Vec2(x, y);
	//groundBodyDef.userData = (void*)(this);
	
	body = world->CreateBody(&groundBodyDef);
	
	b2PolygonShape groundBox;
	groundBox.SetAsBox(width/2, height/2);

	b2FixtureDef FixtureDef;
	FixtureDef.density = 0.5f;
    FixtureDef.friction = 0.2f;
	FixtureDef.restitution = 0.7f;
	FixtureDef.shape = &groundBox;
    //FixtureDef.userData = (void*)(this);
	
	// The extents are the half-widths of the box.

	// Add the ground fixture to the ground body.
	body->CreateFixture(&groundBox, 0.0f);
	body->SetUserData(this);
	
}

Wall::~Wall()
{
	body->GetWorld()->DestroyBody(body);
	world = nullptr;
}















