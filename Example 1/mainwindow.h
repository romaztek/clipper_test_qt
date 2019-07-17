#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsPolygonItem>
#include <QMouseEvent>

#include <math.h>

#include <clipper.hpp>

using namespace ClipperLib;

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

    void Circle(int _x, int _y, int _r, Paths *path);

private:
    Ui::MainWindow *ui;
    Paths *mainPath;
    QGraphicsTextItem *textItem;

    QPen pen;
    QBrush brush;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H



