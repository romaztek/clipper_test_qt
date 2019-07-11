#include "mainwindow.h"
#include "ui_mainwindow.h"

Paths my_subj(1);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(0, 0, 640, 480);

    QGraphicsView *view = this->findChild<QGraphicsView*>("graphicsView");
    view->setScene(scene);

    mainPath = new Paths(1);
    mainPath->at(0) << IntPoint(100, 100) << IntPoint(500, 100) <<
                   IntPoint(500, 400) << IntPoint(100, 400);

    QPolygon mainPath_poly;
    for(unsigned int i = 0; i < mainPath->at(0).size(); i++) {
        mainPath_poly.append(QPoint((int)mainPath->at(0).at(i).X, (int)mainPath->at(0).at(i).Y));
    }

    pen.setCosmetic(true);
    pen.setColor(Qt::darkMagenta);
    pen.setWidth(2);

    scene->addPolygon(mainPath_poly, pen, QBrush(Qt::magenta));

    textItem = new QGraphicsTextItem;
    textItem->setPlainText(QString("Number of polygons: ") + QString::number(1));
    textItem->setPos(5, 5);
    scene->addItem(textItem);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Circle(int _x, int _y, int _r, Paths *path)
{
    float x, y, r, n, dn;
    int x1, y1;
    x = _x;
    y = _y;
    r = _r;

    dn = 0.1/r;
    n = 0;
    while (n < 2*M_PI)
    {
        x1 = round(x + r*cos(n));
        y1 = round(y + r*sin(n));

        path[0][0] << IntPoint(x1, y1);

        n = n + dn;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    int g_x = event->pos().x();
    int g_y = event->pos().y();
    qDebug().noquote() << "Mouse x/y:" << g_x << g_y;

    Paths circlePath(1), new_solution;


    Circle(g_x, g_y, 40, &circlePath);

    Clipper c;
    c.AddPaths(*mainPath, ptSubject, true);
    c.AddPaths(circlePath, ptClip, true);
    c.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);

    *mainPath = new_solution;

    QPolygon solution_poly[new_solution.size()];

    for(unsigned int i = 0; i < new_solution.size(); i++) {
        for(unsigned int j = 0; j < new_solution[i].size(); j++) {
            solution_poly[i].append(QPoint((int)new_solution[i][j].X, (int)new_solution[i][j].Y));
        }
    }

    scene->clear();
    for(unsigned int i = 0; i < new_solution.size(); i++) {
        scene->addPolygon(solution_poly[i], pen, QBrush(Qt::magenta));
    }

    //scene->addEllipse(g_x - 5, g_y - 5, 10, 10, QPen(Qt::black), QBrush(Qt::gray));

    qDebug().noquote() << new_solution.size() << "polygons";

    textItem = new QGraphicsTextItem;
    textItem->setPlainText(QString("Number of polygons: ") + QString::number(new_solution.size()));
    textItem->setPos(5, 5);
    scene->addItem(textItem);

}
