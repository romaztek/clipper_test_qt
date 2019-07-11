#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QObject>

class MyScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MyScene(int x, int y, int width, int height);

signals:
    void sendPos(int s_x, int s_y);


    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // MYSCENE_H
