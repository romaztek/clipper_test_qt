#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _USE_MATH_DEFINES

#include <math.h>

#include <QDebug>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsPolygonItem>
#include <QMouseEvent>

#include <clipper.hpp>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QGraphicsScene *scene = nullptr;

    void processTheTerrain(int mouse_x, int mouse_y);

    void setCircleToPath(int _x, int _y, int _r, ClipperLib::Path *path);

private:
    Ui::MainWindow *ui;

    ClipperLib::Paths *mainPathArray;

    QGraphicsTextItem *polygonCountTextItem;

    QVector<QGraphicsPolygonItem*> polygonItemVector;

    QPen pen;
    QBrush brush;

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H



