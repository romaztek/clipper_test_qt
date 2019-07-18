#ifndef POLYGONBODY_H
#define POLYGONBODY_H

#include <iostream>
#include <QDebug>

#include <Box2D/Box2D.h>
#include "ghost.h"

extern const int SCALE;

class PolygonBody : public Ghost
{
    public:
        PolygonBody(b2Vec2 *_vertices, size_t _count, b2Vec2 _position, b2World *_world);
        ~PolygonBody();

        size_t count;
        b2World *world;
        b2Body *body;

    private:

};


#endif // POLYGONBODY_H
