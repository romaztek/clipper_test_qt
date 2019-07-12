#include "mainwindow.h"
#include "ui_mainwindow.h"


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

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::magenta);

    scene->addPolygon(mainPath_poly, pen, brush);

    textItem = scene->addText(QString("Number of polygons: ") + QString::number(1));
    textItem->setPos(5, 5);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Circle(int _x, int _y, int _r, Paths *path)        // Create a circle polygon Path
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
    event->accept();                    // Accept mouse event
    int mouse_x = event->pos().x();     // Store mouse X
    int mouse_y = event->pos().y();     // Store mouse Y

    // Print mouse coordinates to console
    qDebug().noquote() << "Mouse x/y:" << mouse_x << mouse_y;

    Paths circlePath(1);    // Paths for circle's path
    Paths new_solution;     // Paths for proceeded polygon after clipping

    Circle(mouse_x, mouse_y, 40, &circlePath);  // Clipping circle, position based on mouse position

    // Clipping
    Clipper c;
    c.AddPaths(*mainPath, ptSubject, true);
    c.AddPaths(circlePath, ptClip, true);
    c.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);

    *mainPath = new_solution;       // After clipping: clipped polygon = main polygon (current polygon)

    // Polygons array for drawing
    QPolygon solution_poly[new_solution.size()];

    for(unsigned int i = 0; i < new_solution.size(); i++)
    {
        for(unsigned int j = 0; j < new_solution[i].size(); j++)
        {
            // Add points from Paths to polygons
            solution_poly[i].append(QPoint((int)new_solution[i][j].X, (int)new_solution[i][j].Y));
        }
    }

    scene->clear();     // Clear scene

    // Draw all polygons after clipping
    for(unsigned int i = 0; i < new_solution.size(); i++)
    {
        scene->addPolygon(solution_poly[i], pen, brush);
    }

    // Uncomment if you want to see a position of last mouse press
    //scene->addEllipse(mouse_x - 5, mouse_y - 5, 10, 10, QPen(Qt::black), QBrush(Qt::gray));

    // Draw number of polygons
    textItem = scene->addText(QString("Number of polygons: ") + QString::number(new_solution.size()));
    textItem->setPos(5, 5);

    qDebug().noquote() << new_solution.size() << "polygons";  // Print number of polygons to console

}
