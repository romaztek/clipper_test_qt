#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
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

#include <math.h>

#include <clipper.hpp>
#include <Box2D/Box2D.h>

#include "wall.h"
#include "polygonbody.h"

extern const int SCALE;

using namespace ClipperLib;

namespace Ui {
class MainWindow;
}

class Circle : public QGraphicsEllipseItem
{
public:
    Circle(qreal radius, QPointF initPos, b2World *world);
    ~Circle();
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

    void setCirclePath(int _x, int _y, int _r, Paths *path);

    void resetPolygon();
    void repaintPolygon();

private:
    Ui::MainWindow *ui;
    Paths *mainPath;

    QGraphicsTextItem *textItem;
    QVector<QGraphicsPolygonItem*> polygonItem;

    QPen pen;
    QBrush brush;

    bool mouseLeftKeyPressed = false;
    bool mouseRightKeyPressed = false;

    bool reseted = true;
    bool busy = false;

    int brush_width = 16;

    QTimer *frameTimer;

    b2World *world;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_horizontalSlider_valueChanged(int value);
    void on_resetButton_clicked();
    void on_radioButtonMagenta_toggled(bool checked);
    void on_radioButtonGreen_toggled(bool checked);
    void on_radioButtonRed_toggled(bool checked);
    void on_radioButtonOrange_toggled(bool checked);
    void on_radioButtonSteelBlue_toggled(bool checked);
};


#endif // MAINWINDOW_H

