#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 *
 * This example does not delete a duplicate points, it  may cause qpolygon issues
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
    this->move(710, 60);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    scene = new QGraphicsScene(0, 0, 640, 480);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setAttribute(Qt::WA_TransparentForMouseEvents);

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
    polygonItem.append(scene->addPolygon(drawingPolygon, pen, brush));

    polygonCountTextItem = scene->addText(QString("Number of polygons: ") + QString::number(mainPathArray->size()));
    polygonCountTextItem->setPos(5, 5);

}

MainWindow::~MainWindow()  // Destructor
{
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();

    foreach (QGraphicsPolygonItem *tmp_poly, polygonItem)
    {
        scene->removeItem(tmp_poly);
        delete tmp_poly;
    }

    polygonItem.clear();
    polygonItem.squeeze();

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

void MainWindow::reset()           // Delete mainPathArray and repaint
{
    // Clear mainPathArray
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();

    // Clear old polygons from scene and free memory
    foreach (QGraphicsPolygonItem *tmp_poly, polygonItem)
    {
        scene->removeItem(tmp_poly);
        delete tmp_poly;                    // Dont know, need or not
    }
    polygonItem.clear();
    polygonItem.squeeze();

    malloc_trim(0);     // Release free memory from the heap to OS

    // Recreate mainPathArray and fill it with Rectangle
    mainPathArray = new ClipperLib::Paths(1);
    mainPathArray->at(0) << ClipperLib::IntPoint(100, 100) << ClipperLib::IntPoint(500, 100) <<
                   ClipperLib::IntPoint(500, 400) << ClipperLib::IntPoint(100, 400);

    for(unsigned int i = 0; i < mainPathArray->size(); i++)
    {
        QPolygon drawingPolygon;
        for(unsigned int j = 0; j < mainPathArray->at(i).size(); j++)
        {
            // Add points from ClipperLib::Paths to QPolygon
            drawingPolygon.append(QPoint((int)mainPathArray->at(i).at(j).X, (int)mainPathArray->at(i).at(j).Y));
        }

        // Add QGraphicsPolygonItem with QPolygon to QVector<QGraphicsPolygonItem*>
        polygonItem.append(scene->addPolygon(drawingPolygon, pen, brush));
    }

    qDebug().noquote() << mainPathArray->size() << "polygons";  // Print number of polygons to console

    polygonCountTextItem->setPlainText(QString("Number of polygons: ") + QString::number(mainPathArray->size()));

}

void MainWindow::repaintPolygon()           // Just repaint mainPathArray without delete
{
    if(mainPathArray->empty())
    {
        qDebug().noquote() << "mainPathArray is empty. Nothing to repaint.";
        return;
    }

    // Clear old polygons from scene and free memory
    foreach (QGraphicsPolygonItem *tmp_poly, polygonItem)
    {
        scene->removeItem(tmp_poly);
        delete tmp_poly;                    // Dont know, need or not
    }
    polygonItem.clear();
    polygonItem.squeeze();

    for(unsigned int i = 0; i < mainPathArray->size(); i++)
    {
        QPolygon drawingPolygon;
        for(unsigned int j = 0; j < mainPathArray->at(i).size(); j++)
        {
            // Add points from ClipperLib::Paths to QPolygon
            drawingPolygon.append(QPoint((int)mainPathArray->at(i).at(j).X, (int)mainPathArray->at(i).at(j).Y));
        }
        // Add QGraphicsPolygonItem with QPolygon to QVector<QGraphicsPolygonItem*>
        polygonItem.append(scene->addPolygon(drawingPolygon, pen, brush));
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

    setCircleToPath(mouse_x, mouse_y, brush_width, &circlePath);  // Clipping circle, position based on mouse position

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
    foreach (QGraphicsPolygonItem *tmp_poly, polygonItem)
    {
        scene->removeItem(tmp_poly);
        delete tmp_poly;                    // Dont know, need or not
    }
    qDebug().noquote() << "polygonItem bytes before clear :" << polygonItem.size() * sizeof(polygonItem.at(0));
    polygonItem.clear();
    polygonItem.squeeze();
    qDebug().noquote() << "polygonItem bytes after clear:" << polygonItem.size() * sizeof(polygonItem.at(0));

    for(unsigned int i = 0; i < mainPathArray->size(); i++)
    {
        QPolygon drawingPolygon;
        for(unsigned int j = 0; j < mainPathArray->at(i).size(); j++)
        {
            // Add points from ClipperLib::Paths to QPolygon
            drawingPolygon.append(QPoint((int)mainPathArray->at(i).at(j).X, (int)mainPathArray->at(i).at(j).Y));
        }
        // Add QGraphicsPolygonItem with QPolygon to QVector<QGraphicsPolygonItem*>
        polygonItem.append(scene->addPolygon(drawingPolygon, pen, brush));
    }

    qDebug().noquote() << "polygonItem bytes after append :" << polygonItem.size() * sizeof(polygonItem.at(0));
    qDebug().noquote() << mainPathArray->size() << "polygons";  // Print number of polygons to console

    polygonCountTextItem->setPlainText(QString("Number of polygons: ") + QString::number(mainPathArray->size()));
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // Accept mouse event
    event->accept();

    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseLeftKeyPressed = true;
    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
        mouseRightKeyPressed = true;
    }

    if(mouseLeftKeyPressed)
    {
        processTheTerrain(event->pos().x(), event->pos().y());
    }

    // Pass the event further
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // If left mouse key not pressed ignore event
    if(!mouseLeftKeyPressed)
    {
        event->ignore();
    }
    else
    {
        event->accept();
        processTheTerrain(event->pos().x(), event->pos().y());
    }

    // Pass the event further
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // Accept mouse event
    event->accept();

    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseLeftKeyPressed = false;
    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
        mouseRightKeyPressed = false;
    }

    // Pass the event further
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->width_label->setText(QString::number(value));
    brush_width = value;
}

void MainWindow::on_resetButton_clicked()
{
    reset();
}

void MainWindow::on_radioButtonMagenta_toggled(bool checked)
{
    if(checked)
    {
        brush.setColor(Qt::magenta);
        pen.setColor(Qt::darkMagenta);
    }
    repaintPolygon();
}


void MainWindow::on_radioButtonGreen_toggled(bool checked)
{
    if(checked)
    {
        brush.setColor(Qt::green);
        pen.setColor(Qt::darkGreen);
    }
    repaintPolygon();
}

void MainWindow::on_radioButtonRed_toggled(bool checked)
{
    if(checked)
    {
        brush.setColor(Qt::red);
        pen.setColor(Qt::darkRed);
    }
    repaintPolygon();
}


void MainWindow::on_radioButtonOrange_toggled(bool checked)
{
    if(checked)
    {
        brush.setColor(QColor(255, 165, 0, 255));
        pen.setColor(QColor(238, 118, 0, 255));
    }
    repaintPolygon();
}

void MainWindow::on_radioButtonSteelBlue_toggled(bool checked)
{
    if(checked)
    {
        brush.setColor(QColor(176, 196, 222, 255));
        pen.setColor(QColor(70, 130, 180, 255));
    }
    repaintPolygon();
}





