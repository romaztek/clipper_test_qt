#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    scene = new QGraphicsScene(0, 0, 640, 480);

    QGraphicsView *view = this->findChild<QGraphicsView*>("graphicsView");
    view->setScene(scene);

    view->viewport()->setFocus(Qt::MouseFocusReason);
    view->viewport()->setAttribute(Qt::WA_TransparentForMouseEvents);

    scene->setFocus(Qt::MouseFocusReason);

    view->setFocus(Qt::MouseFocusReason);
    view->setFocusPolicy(Qt::StrongFocus);
    view->setMouseTracking(true);
    this->installEventFilter(view);

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

    polygonItem.append(scene->addPolygon(mainPath_poly, pen, brush));

    textItem = scene->addText(QString("Number of polygons: ") + QString::number(1));
    textItem->setPos(5, 5);

    qDebug().noquote() << "reseted" << reseted;
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

void MainWindow::resetPolygon()
{
    mainPath->clear();
    mainPath = new Paths(1);
    mainPath->at(0) << IntPoint(100, 100) << IntPoint(500, 100) <<
                   IntPoint(500, 400) << IntPoint(100, 400);

    QPolygon mainPath_poly;
    for(unsigned int i = 0; i < mainPath->at(0).size(); i++) {
        mainPath_poly.append(QPoint((int)mainPath->at(0).at(i).X, (int)mainPath->at(0).at(i).Y));
    }

    foreach (QGraphicsPolygonItem *tmp_poly, polygonItem) {
        scene->removeItem(tmp_poly);
    }

    polygonItem.clear();

    for(unsigned int i = 0; i < mainPath->at(0).size(); i++) {
        polygonItem.append(scene->addPolygon(mainPath_poly, pen, brush));
    }

    scene->removeItem(textItem);

    textItem = scene->addText(QString("Number of polygons: ") + QString::number(mainPath->size()));
    textItem->setPos(5, 5);

    qDebug().noquote() << mainPath->size() << "polygons";  // Print number of polygons to console
}

void MainWindow::repaintPolygon()
{
    busy = true;
    if(reseted)
    {
        resetPolygon();
    }
    else
    {
        QPolygon solution_poly[mainPath->size()];

        for(unsigned int i = 0; i < mainPath->size(); i++)
        {
            for(unsigned int j = 0; j < mainPath->at(i).size(); j++)
            {
                solution_poly[i].append(QPoint((int)mainPath->at(i).at(j).X,
                                               (int)mainPath->at(i).at(j).Y));
            }
        }

        foreach (QGraphicsPolygonItem *tmp_poly, polygonItem) {
            scene->removeItem(tmp_poly);
        }

        polygonItem.clear();

        qWarning().noquote() << "after that";

        // Draw all polygons after clipping
        for(unsigned int i = 0; i < mainPath->size(); i++)
        {
            polygonItem.append(scene->addPolygon(solution_poly[i], pen, brush));
        }
    }
    busy = false;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();                    // Accept mouse event
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseLeftKeyPressed = true;
        reseted = false;
    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
        mouseRightKeyPressed = true;
    }

    if(mouseLeftKeyPressed  && !busy)
    {
        int mouse_x = event->pos().x();     // Store mouse X
        int mouse_y = event->pos().y();     // Store mouse Y

        // Print mouse coordinates to console
        qDebug().noquote() << "Mouse x/y:" << mouse_x << mouse_y;

        Paths circlePath(1);    // Paths for circle's path
        Paths new_solution;     // Paths for proceeded polygon after clipping

        Circle(mouse_x, mouse_y, brush_width, &circlePath);  // Clipping circle, position based on mouse position

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

        foreach (QGraphicsPolygonItem *tmp_poly, polygonItem) {
            scene->removeItem(tmp_poly);
        }

        polygonItem.clear();

        // Draw all polygons after clipping
        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            polygonItem.append(scene->addPolygon(solution_poly[i], pen, brush));
        }

        // Uncomment if you want to see a position of last mouse press
        //scene->addEllipse(mouse_x - 5, mouse_y - 5, 10, 10, QPen(Qt::black), QBrush(Qt::gray));

        // Remove and redraw number of polygons
        scene->removeItem(textItem);

        textItem = scene->addText(QString("Number of polygons: ") + QString::number(new_solution.size()));
        textItem->setPos(5, 5);


        qDebug().noquote() << new_solution.size() << "polygons";  // Print number of polygons to console

    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    if(mouseLeftKeyPressed && !busy) {
        int mouse_x = event->pos().x();     // Store mouse X
        int mouse_y = event->pos().y();     // Store mouse Y

        // Print mouse coordinates to console
        qDebug().noquote() << "Mouse x/y:" << mouse_x << mouse_y;

        Paths circlePath(1);    // Paths for circle's path
        Paths new_solution;     // Paths for proceeded polygon after clipping

        Circle(mouse_x, mouse_y, brush_width, &circlePath);  // Clipping circle, position based on mouse position

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

        foreach (QGraphicsPolygonItem *tmp_poly, polygonItem) {
            scene->removeItem(tmp_poly);
        }

        polygonItem.clear();

        // Draw all polygons after clipping
        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            polygonItem.append(scene->addPolygon(solution_poly[i], pen, brush));
        }

        // Uncomment if you want to see a position of last mouse press
        //scene->addEllipse(mouse_x - 5, mouse_y - 5, 10, 10, QPen(Qt::black), QBrush(Qt::gray));

        // Remove and redraw number of polygons
        scene->removeItem(textItem);

        textItem = scene->addText(QString("Number of polygons: ") + QString::number(new_solution.size()));
        textItem->setPos(5, 5);

        qDebug().noquote() << new_solution.size() << "polygons";  // Print number of polygons to console
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseLeftKeyPressed = false;
    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
        mouseRightKeyPressed = false;
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->width_label->setText(QString::number(value));
    brush_width = value;
}

void MainWindow::on_resetButton_clicked()
{
    reseted = true;
    resetPolygon();
}

void MainWindow::on_radioButtonMagenta_toggled(bool checked)
{
    if(checked) {
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
    if(checked) {
        brush.setColor(Qt::red);
        pen.setColor(Qt::darkRed);
    }
    repaintPolygon();
}


void MainWindow::on_radioButtonOrange_toggled(bool checked)
{
    if(checked) {
        brush.setColor(QColor(255, 165, 0, 255));
        pen.setColor(QColor(238, 118, 0, 255));
    }
    repaintPolygon();
}

void MainWindow::on_radioButtonSteelBlue_toggled(bool checked)
{
    if(checked) {
        brush.setColor(QColor(176, 196, 222, 255));
        pen.setColor(QColor(70, 130, 180, 255));
    }
    repaintPolygon();
}




