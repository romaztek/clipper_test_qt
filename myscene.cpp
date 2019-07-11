#include "myscene.h"

MyScene::MyScene(int x, int y, int width, int height)
{
    this->setSceneRect(x, y, width, height);
}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //QGraphicsScene::mousePressEvent(event);
    //event->accept();
    //emit sendPos(event->pos().x(), event->pos().y());
}
