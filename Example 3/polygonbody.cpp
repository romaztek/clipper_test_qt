#include "polygonbody.h"

PolygonBody::PolygonBody(b2Vec2 *_vertices, size_t _count, b2Vec2 _position, b2World *_world) :
    world(_world),
    vertices(_vertices),
    count(_count)
{
    /*unsigned int step = (unsigned int)(count/8);

    for(unsigned int i = 0; i < count; i++) {

        //b2PolygonShape polygonShape;
        //polygonShape.Set(_vertices, _count);
    }*/

    b2PolygonShape polygonShape;
    polygonShape.Set(_vertices, _count); //pass array to the shape

    //qDebug().noquote() << "passed to polydonBody count|x|y" << _count << _position.x << _position.y;

    /*for(unsigned int i = 0; i < count; i++) {
        qDebug().noquote() << "passed to polydonBody vertices" << _vertices[i].x << _vertices[i].y;
    }*/

    b2FixtureDef myFixtureDef;
    myFixtureDef.density = 0.2f;
    myFixtureDef.friction = 0.2f;
    myFixtureDef.restitution = 0.7f;
    myFixtureDef.shape = &polygonShape; //change the shape of the fixture
    myFixtureDef.userData = (void*)(10);

    b2BodyDef myBodyDef;
    myBodyDef.type = b2_staticBody; //this will be a dynamic body
    //myBodyDef.userData = (void*)(10);
    myBodyDef.position.Set(_position.x, _position.y); //in the middle

    body = _world->CreateBody(&myBodyDef);
    body->CreateFixture(&myFixtureDef); //add a fixture to the body
}

PolygonBody::~PolygonBody()
{
    body->GetWorld()->DestroyBody(body);
    vertices = nullptr;
    world = nullptr;
}


