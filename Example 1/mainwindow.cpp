#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 *
 * This example does not delete duplicate points, it may cause qpolygon issues
 * (and poly2tri segfault, if you will using with it in this example)
 * Example 3 delete duplicate points using std::set
 *
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    scene = new QGraphicsScene(0, 0, 640, 480);

    ui->graphicsView->setScene(scene);

    pen.setCosmetic(true);
    pen.setColor(Qt::darkMagenta);
    pen.setWidth(2);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::magenta);

    // Create mainPathArray and fill it with Rectangle
    mainPathArray = new ClipperLib::Paths(1);
    mainPathArray->at(0) << ClipperLib::IntPoint(100, 100) << ClipperLib::IntPoint(500, 100) <<
                   ClipperLib::IntPoint(500, 400) << ClipperLib::IntPoint(100, 400);

    QPolygon drawingPolygon;
    for(unsigned int i = 0; i < mainPathArray->at(0).size(); i++)
    {
        drawingPolygon.append(QPoint((int)mainPathArray->at(0).at(i).X, (int)mainPathArray->at(0).at(i).Y));
    }
    polygonItemVector.append(scene->addPolygon(drawingPolygon, pen, brush));

    polygonCountTextItem = scene->addText(QString("Number of polygons: ") + QString::number(mainPathArray->size()));
    polygonCountTextItem->setPos(5, 5);
}

MainWindow::~MainWindow()
{
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();

    foreach (QGraphicsPolygonItem *polygonItem, polygonItemVector)
    {
        scene->removeItem(polygonItem);
        delete polygonItem;
    }

    polygonItemVector.clear();
    polygonItemVector.squeeze();

    scene->removeItem(polygonCountTextItem);
    delete polygonCountTextItem;

    delete scene;
    delete ui;
    delete mainPathArray;
}

void MainWindow::setCircleToPath(int _x, int _y, int _r, ClipperLib::Path *path)        // Create a circle polygon Path
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

        path->push_back(ClipperLib::IntPoint(x1, y1));

        n = n + dn;
    }
}

void MainWindow::processTheTerrain(int mouse_x, int mouse_y)
{
    if(mainPathArray->empty())
    {
        qDebug().noquote() << "mainPathArray is empty. Do nothing.";
        return;
    }

    qDebug().noquote() << "mainPathArray bytes:" << mainPathArray->size() * sizeof(mainPathArray->at(0));

    ClipperLib::Path circlePath;         // Paths for circle's path
    ClipperLib::Paths executedArray;     // Paths for proceeded polygon after clipping

    setCircleToPath(mouse_x, mouse_y, 40, &circlePath);  // Clipping circle, position based on mouse position

    // Clipping
    ClipperLib::Clipper c;
    c.AddPaths(*mainPathArray, ClipperLib::ptSubject, true);
    c.AddPath(circlePath, ClipperLib::ptClip, true);
    c.Execute(ClipperLib::ctDifference, executedArray, ClipperLib::pftEvenOdd , ClipperLib::pftEvenOdd);
    c.Clear();

    // Clear mainPathArray
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();

    qDebug().noquote() << "mainPathArray bytes after clear:" << mainPathArray->size() * sizeof(mainPathArray->at(0));

    // Copy executedArray to mainPathArray
    *mainPathArray = executedArray;

    // Clear executedArray
    executedArray.clear();
    executedArray.shrink_to_fit();
    qDebug().noquote() << "executedArray bytes after clear:" << executedArray.size() * sizeof(executedArray.at(0));

    // Clear old polygons from scene and free memory
    foreach (QGraphicsPolygonItem *polyItem, polygonItemVector)
    {
        scene->removeItem(polyItem);
        delete polyItem;                    // Dont know, need or not
    }
    qDebug().noquote() << "polygonItemVector bytes before clear :" << polygonItemVector.size() * sizeof(polygonItemVector.at(0));
    polygonItemVector.clear();
    polygonItemVector.squeeze();
    qDebug().noquote() << "polygonItemVector bytes after clear:" << polygonItemVector.size() * sizeof(polygonItemVector.at(0));

    for(unsigned int i = 0; i < mainPathArray->size(); i++)
    {
        QPolygon drawingPolygon;
        for(unsigned int j = 0; j < mainPathArray->at(i).size(); j++)
        {
            // Add points from ClipperLib::Paths to QPolygon
            drawingPolygon.append(QPoint((int)mainPathArray->at(i).at(j).X, (int)mainPathArray->at(i).at(j).Y));
        }
        // Add QGraphicsPolygonItem with QPolygon to QVector<QGraphicsPolygonItem*>
        polygonItemVector.append(scene->addPolygon(drawingPolygon, pen, brush));
    }

    qDebug().noquote() << "polygonItemVector bytes after append :" << polygonItemVector.size() * sizeof(polygonItemVector.at(0));
    qDebug().noquote() << mainPathArray->size() << "polygons";  // Print number of polygons to console

    polygonCountTextItem->setPlainText(QString("Number of polygons: ") + QString::number(mainPathArray->size()));
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // Accept mouse event
    event->accept();

    int mouse_x = event->pos().x();     // Store mouse X
    int mouse_y = event->pos().y();     // Store mouse Y

    qDebug().noquote() << "Mouse x/y:" << mouse_x << mouse_y;

    processTheTerrain(mouse_x, mouse_y);

    // Pass the event further
    QMainWindow::mouseMoveEvent(event);
}
