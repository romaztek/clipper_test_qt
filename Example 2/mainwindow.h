#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
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

    int brush_width = 20;

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



