#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <malloc.h>

#include <QDebug>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
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
    void reset();
    void repaintPolygon();
    void setCircleToPath(int _x, int _y, int _r, ClipperLib::Path *path);


private:
    Ui::MainWindow *ui;

    ClipperLib::Paths *mainPathArray;

    QGraphicsTextItem *polygonCountTextItem;

    QVector<QGraphicsPolygonItem*> polygonItem;

    QPen pen;
    QBrush brush;

    int brush_width = 20;

    bool mouseLeftKeyPressed = false;
    bool mouseRightKeyPressed = false;


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



