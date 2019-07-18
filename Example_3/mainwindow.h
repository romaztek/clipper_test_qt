#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <malloc.h>

#include <stdio.h>
#include <set>
#include <iterator>
#include <chrono>

#include <QObject>
#include <QDebug>
#include <QWidget>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QMouseEvent>
#include <QTimer>
#include <QPointF>

#include <Box2D/Box2D.h>
#include <clipper.hpp>
#include <poly2tri.h>

#include "wall.h"
#include "polygonbody.h"

extern const int SCALE;

using namespace ClipperLib;

namespace Ui {
class MainWindow;
}

class Circle : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    Circle(qreal radius, QPointF initPos, b2World *world);
    ~Circle();
    b2Body *getBody() { return body; }

private:
    b2Body *body;

public:
    virtual void advance(int phase);

};

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    Scene(qreal x, qreal y, qreal width, qreal height, b2World *world);

    bool locked = false;

public slots:
    void advance();

private:
    b2World *world;

    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Scene *scene = nullptr;

    void setCircleToPath(int _x, int _y, int _r, ClipperLib::Path *path) ;

    void reset();
    void repaintPolygon();

    void processTheTerrain(int mouse_x, int mouse_y);

private:
    Ui::MainWindow *ui;
    Paths *mainPathArray;

    b2World *world;

    QPen pen;
    QBrush brush;

    QTimer *frameTimer;

    std::vector<PolygonBody*> myPolygonBodies;

    QVector<QGraphicsPolygonItem*> polygonItemVector;

    QVector<QGraphicsPolygonItem*> triangleItemVector;

    QGraphicsRectItem *groundRect;

    long resultProcessTime = 0;
    long minimumProcessTime = LONG_MAX;
    long maximumProcessTime = LONG_MIN;

    bool busy = false;
    bool draw_triangles = false;
    bool mouseLeftKeyPressed = false;
    bool mouseRightKeyPressed = false;

    int brush_width = 16;
    int numberOfTriangles = 0;
    int numberOfThrownDuplicatePoints = 0;


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_resetButton_clicked();

    void on_radioButtonMagenta_clicked();
    void on_radioButtonGreen_clicked();
    void on_radioButtonRed_clicked();
    void on_radioButtonOrange_clicked();
    void on_radioButtonSteelBlue_clicked();
    void on_radioButtonCherry_clicked();
    void on_radioButtonWhite_clicked();
    void on_radioButtonGrey_clicked();

    void on_radioButtonYes_clicked();
    void on_radioButtonNo_clicked();
};


#endif // MAINWINDOW_H

