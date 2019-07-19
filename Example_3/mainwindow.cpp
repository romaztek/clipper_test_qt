#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "horizontalsliderstylesheet.cpp"

#define ERASER_WIDTH 18
#define CIRCLE_RADIUS 8

extern const QString horizontalSliderStyleSheet;

// Global variables start
bool need_delete_circles = false;
int numberOfCircles = 0;

QGraphicsSimpleTextItem *informationTextItem;
QGraphicsSimpleTextItem *numberOfPolygonsTextItem;
QGraphicsSimpleTextItem *numberOfTrianglesTextItem;
QGraphicsSimpleTextItem *numberOfThrownDuplicatePointsTextItem;
QGraphicsSimpleTextItem *numberOfCircleBodiesTextItem;

QGraphicsSimpleTextItem *lastProcessDurationTextItem;
QGraphicsSimpleTextItem *minimumProcessDurationTextItem;
QGraphicsSimpleTextItem *maximumProcessDurationTextItem;

QVector<Circle*> coloredCirclesVector;

// Global variables end

qreal fromB2(qreal value) {
    return value*SCALE;
}

qreal toB2(qreal value) {
    return value/SCALE;
}

auto compPoint = [&](p2t::Point* const pointA, p2t::Point* const pointB) -> bool
{
    if (pointA->x < pointB->x) return true;
    if (pointA->x > pointB->x) return false;
    if (pointA->y < pointB->y) return true;
    return false;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if ((defined Q_OS_WIN) || defined Q_OS_LINUX) && !Q_OS_ANDROID
    this->move(710, 60);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
#endif

#ifdef Q_OS_ANDROID

#endif

    ui->horizontalSlider->setStyleSheet(horizontalSliderStyleSheet);
    ui->horizontalSlider->setValue(ERASER_WIDTH);

    b2Vec2 gravity(0.0f, 10.0f);
    world = new b2World(gravity);

    scene = new Scene(0, 0, toB2(640), toB2(480), world);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setAttribute(Qt::WA_TransparentForMouseEvents);

    int startColor = qrand() % (ui->ColorGroup->buttons().count());
    /*for(int i=0; i < ui->ColorGroup->buttons().count(); i++) {
        if(i == startColor)
            ui->ColorGroup->buttons().at(i)->setChecked(true);
    }*/

    switch (startColor)
    {
    case 0:
        brush.setColor(Qt::magenta);
        pen.setColor(Qt::darkMagenta);
        break;
    case 1:
        brush.setColor(Qt::green);
        pen.setColor(Qt::darkGreen);
        break;
    case 2:
        brush.setColor(Qt::red);
        pen.setColor(Qt::darkRed);
        break;
    case 3:
        brush.setColor(QColor(255, 165, 0, 255));
        pen.setColor(QColor(238, 118, 0, 255));
        break;
    case 4:
        brush.setColor(QColor(176, 196, 222, 255));
        pen.setColor(QColor(70, 130, 180, 255));
        break;
    case 5:
        brush.setColor(QColor(145, 30, 66, 255));
        pen.setColor(QColor(102, 21, 47, 255));
        break;
    case 6 :
        brush.setColor(QColor(255, 255, 255, 255));
        pen.setColor(QColor(128, 128, 128, 255));
        break;
    case 7:
        brush.setColor(QColor(128, 128, 128, 255));
        pen.setColor(QColor(102, 102, 102, 255));
        break;
    default :
        brush.setColor(Qt::magenta);
        pen.setColor(Qt::darkMagenta);
        break;
    }
    pen.setWidth(2);
    //brush.setStyle(Qt::CrossPattern);
    //brush.setStyle(Qt::SolidPattern);

    /*QPixmap pix(80, 20);
    pix.fill(Qt::transparent);
    QPainter paint(&pix);
    paint.setPen("black");
    paint.setOpacity(0.75);
    paint.drawText(10, 10, "Hello!");
    brush.setTexture(pix);*/

    QPixmap cheeseTexture(":/cheese.png");
    brush.setTexture(cheeseTexture);

    mainPathArray = new Paths(1);
    mainPathArray->at(0) << IntPoint(40, 200) << IntPoint(600, 200) <<
                   IntPoint(600, 420) << IntPoint(40, 420);


    QPen groundRectPen;
    groundRectPen.setCosmetic(true);
    groundRectPen.setColor(Qt::black);
    groundRect = new QGraphicsRectItem(0, this->height() - 80, this->width() - 1,  20 - 2);
    groundRect->setPen(groundRectPen);
    groundRect->setBrush(QBrush(Qt::yellow));

    scene->addItem(groundRect);

    const float bottomGroundX_B2 = toB2(this->width()/2);
    const float bottomGroundY_B2 = toB2(this->height() - 80 + 20/2);
    const float bottomGroundWidth_B2 = toB2(640);
    const float bottomGroundHeight_B2 = toB2(20);

    bottomGround = new Wall(bottomGroundX_B2, bottomGroundY_B2,
                                  bottomGroundWidth_B2, bottomGroundHeight_B2, 10, world);

    b2Vec2 myPolygonBodyVertices[mainPathArray->at(0).size()];

    QPolygon drawingPolygon;

    for(unsigned int i = 0; i < mainPathArray->at(0).size(); i++)
    {
        drawingPolygon.append(QPoint((int)mainPathArray->at(0).at(i).X, (int)mainPathArray->at(0).at(i).Y));

        myPolygonBodyVertices[i].x = toB2((int)mainPathArray->at(0).at(i).X);
        myPolygonBodyVertices[i].y = toB2((int)mainPathArray->at(0).at(i).Y);
    }

    myPolygonBodies.push_back(new PolygonBody(myPolygonBodyVertices, 4, b2Vec2(0, 0), world));

    polygonItemVector.append(scene->addPolygon(drawingPolygon, pen, brush));

    // Draw information
    informationTextItem = scene->addSimpleText(QString("Left click: edit terrain\nRight click: add circle body"));
    informationTextItem->setPos(this->width() - 5 - informationTextItem->boundingRect().width(), 5);

    // Draw number of circle bodies
    numberOfCircleBodiesTextItem = scene->addSimpleText(QString("Number of circle bodies: 0"));
    numberOfCircleBodiesTextItem->setPos(this->width() - 5 - numberOfCircleBodiesTextItem->boundingRect().width(), 45);

    // Draw number of polygons
    numberOfPolygonsTextItem = scene->addSimpleText(QString("Number of polygons: 1"));
    numberOfPolygonsTextItem->setPos(5, 5);

    // Draw number of triangles formed from polygons (0 at start)
    numberOfTrianglesTextItem = scene->addSimpleText(QString("Number of triangles formed from polygons: 0"));
    numberOfTrianglesTextItem->setPos(5,25);

    // Draw number of thrown duplicate points in last processing (0 at start)
    numberOfThrownDuplicatePointsTextItem = scene->addSimpleText(QString("Number of thrown duplicate points in last processing: ")
                                                             + QString::number(numberOfThrownDuplicatePoints));
    numberOfThrownDuplicatePointsTextItem->setPos(5, 45);


    lastProcessDurationTextItem = scene->addSimpleText(
                QString("Process duration: \n") + QString("Last: \n") +
                QString("N/A") + QString(" ms\n") +
                QString("N/A") + QString(" µs\n") +
                QString("N/A") + QString(" ns\n"));

    minimumProcessDurationTextItem = scene->addSimpleText(
                QString("\n") + QString("Minimum: \n") +
                QString("N/A") + QString(" ms\n") +
                QString("N/A") + QString(" µs\n") +
                QString("N/A") + QString(" ns\n"));

    maximumProcessDurationTextItem = scene->addSimpleText(
                QString("\n") + QString("Maximum: \n") +
                QString("N/A") + QString(" ms\n") +
                QString("N/A") + QString(" µs\n") +
                QString("N/A") + QString(" ns\n"));

    lastProcessDurationTextItem->setPos(5, 65);
    minimumProcessDurationTextItem->setPos(95, 65);
    maximumProcessDurationTextItem->setPos(185, 65);

    frameTimer = new QTimer(this);
    connect(frameTimer, SIGNAL(timeout()), scene, SLOT(advance()));
    frameTimer->start(1000/60);

}

MainWindow::~MainWindow()  // Destructor
{
    disconnect(frameTimer, SIGNAL(timeout()), scene, SLOT(advance()));
    frameTimer->stop();
    delete frameTimer;

    need_delete_circles = true;

    delete bottomGround;
    delete groundRect;

    for(PolygonBody* myPolygonBody : myPolygonBodies)
    {
        delete myPolygonBody;
    }
    myPolygonBodies.clear();
    myPolygonBodies.shrink_to_fit();

    coloredCirclesVector.clear();
    coloredCirclesVector.squeeze();

    // Clear triangleItemVector
    for(QGraphicsPolygonItem *triangleItem : triangleItemVector) {
        scene->removeItem(triangleItem);
        delete triangleItem;
    }
    triangleItemVector.clear();
    triangleItemVector.squeeze();

    // Clear polygonItemVector
    for(QGraphicsPolygonItem *polygonItem : polygonItemVector) {
        scene->removeItem(polygonItem);
        delete polygonItem;
    }
    polygonItemVector.clear();
    polygonItemVector.squeeze();

    // Clear myPolygonBodies
    for(PolygonBody* myPolygonBody : myPolygonBodies)
    {
        delete myPolygonBody;
    }
    myPolygonBodies.clear();
    myPolygonBodies.shrink_to_fit();

    // Clear mainPathArray
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();
    delete mainPathArray;

    scene->removeItem(informationTextItem);
    scene->removeItem(numberOfPolygonsTextItem);
    scene->removeItem(numberOfTrianglesTextItem);
    scene->removeItem(numberOfThrownDuplicatePointsTextItem);
    scene->removeItem(numberOfCircleBodiesTextItem);

    scene->removeItem(lastProcessDurationTextItem);
    scene->removeItem(minimumProcessDurationTextItem);
    scene->removeItem(maximumProcessDurationTextItem);

    delete informationTextItem;
    delete numberOfPolygonsTextItem;
    delete numberOfTrianglesTextItem;
    delete numberOfThrownDuplicatePointsTextItem;
    delete numberOfCircleBodiesTextItem;

    delete lastProcessDurationTextItem;
    delete minimumProcessDurationTextItem;
    delete maximumProcessDurationTextItem;

    delete scene;
    delete world;
    delete ui;
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

void MainWindow::reset()
{
    // Clear triangleItemVector
    foreach (QGraphicsPolygonItem *triangleItem, triangleItemVector) {
        scene->removeItem(triangleItem);
        delete triangleItem;
    }
    triangleItemVector.clear();
    triangleItemVector.squeeze();

    // Clear polygonItemVector
    foreach (QGraphicsPolygonItem *polygonItem, polygonItemVector) {
        scene->removeItem(polygonItem);
        delete polygonItem;
    }
    polygonItemVector.clear();
    polygonItemVector.squeeze();

    // Clear myPolygonBodies
    for(PolygonBody* myPolygonBody : myPolygonBodies)
    {
        delete myPolygonBody;
    }
    myPolygonBodies.clear();
    myPolygonBodies.shrink_to_fit();

    // Clear mainPathArray
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();
    delete mainPathArray;

#ifdef Q_OS_LINUX
    malloc_trim(0);     // Release free memory from the heap to OS
#endif

    // Recreate mainPathArray and fill it with Rectangle
    mainPathArray = new ClipperLib::Paths(1);
    mainPathArray->at(0) << IntPoint(40, 200) << IntPoint(600, 200) <<
                   IntPoint(600, 420) << IntPoint(40, 420);\

    b2Vec2 myPolygonBodyVertices[mainPathArray->at(0).size()];

    QPolygon drawingPoly;

    for(unsigned int i = 0; i < mainPathArray->at(0).size(); i++)
    {
        drawingPoly.append(QPoint((int)mainPathArray->at(0).at(i).X, (int)mainPathArray->at(0).at(i).Y));

        myPolygonBodyVertices[i].x = toB2((int)mainPathArray->at(0).at(i).X);
        myPolygonBodyVertices[i].y = toB2((int)mainPathArray->at(0).at(i).Y);
    }

    myPolygonBodies.push_back(new PolygonBody(myPolygonBodyVertices, 4, b2Vec2(0, 0), world));

    polygonItemVector.append(scene->addPolygon(drawingPoly, pen, brush));

    // Redraw number of circle bodies
    numberOfCircleBodiesTextItem->setText(QString("Number of circle bodies: 0"));
    numberOfCircleBodiesTextItem->setPos(this->width() - 5 - numberOfCircleBodiesTextItem->boundingRect().width(), 45);

    // Redraw number of polygons
    numberOfPolygonsTextItem->setText(QString("Number of polygons: ") + QString::number(mainPathArray->size()));

    // Redraw number of triangles formed from polygons
    numberOfTriangles = 0;
    numberOfTrianglesTextItem->setText(QString("Number of triangles formed from polygons: ") + QString::number(numberOfTriangles));

    // Redraw number of thrown duplicate points in last processing
    numberOfThrownDuplicatePoints = 0;
    numberOfThrownDuplicatePointsTextItem->setText(QString("Number of thrown duplicate points in last processing: ")
                                                             + QString::number(numberOfThrownDuplicatePoints));

    lastProcessDurationTextItem->setText(
                QString("Process duration: \n") + QString("Last: \n") +
                QString("N/A") + QString(" ms\n") +
                QString("N/A") + QString(" µs\n") +
                QString("N/A") + QString(" ns\n"));

    minimumProcessDurationTextItem->setText(
                QString("\n") + QString("Minimum: \n") +
                QString("N/A") + QString(" ms\n") +
                QString("N/A") + QString(" µs\n") +
                QString("N/A") + QString(" ns\n"));

    maximumProcessDurationTextItem->setText(
                QString("\n") + QString("Maximum: \n") +
                QString("N/A") + QString(" ms\n") +
                QString("N/A") + QString(" µs\n") +
                QString("N/A") + QString(" ns\n"));

}

void MainWindow::repaintPolygon()
{
    if(mainPathArray->empty())
    {
        qDebug().noquote() << "mainPathArray is empty. Nothing to repaint.";
        return;
    }

    // Clear old polygons from scene and free memory
    foreach (QGraphicsPolygonItem *polyItem, polygonItemVector)
    {
        scene->removeItem(polyItem);
        delete polyItem;                    // Dont know, need or not
    }
    polygonItemVector.clear();
    polygonItemVector.squeeze();

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

}

void MainWindow::processTheTerrain(int mouse_x, int mouse_y)
{
    if(mainPathArray->empty())
    {
        qDebug().noquote() << "mainPathArray is empty. Do nothing.";
        return;
    }

    auto start(std::chrono::high_resolution_clock::now());

    ClipperLib::Path circlePath;         // Paths for circle's path
    ClipperLib::Paths executedArray;     // Paths for proceeded polygon after clipping

    setCircleToPath(mouse_x, mouse_y, brush_width, &circlePath);  // Clipping circle, position based on mouse position

    // Clipping
    ClipperLib::Clipper c;
    c.AddPaths(*mainPathArray, ClipperLib::ptSubject, true);
    c.AddPath(circlePath, ClipperLib::ptClip, true);
    c.Execute(ClipperLib::ctDifference, executedArray, ClipperLib::pftEvenOdd , ClipperLib::pftEvenOdd);
    c.Clear();

    bool path_changed_bool = (*mainPathArray != executedArray);
    if(!path_changed_bool)
    {
        //qDebug().noquote() << "Path not changed:";
        circlePath.clear();
        circlePath.shrink_to_fit();
        executedArray.clear();
        executedArray.shrink_to_fit();
        return;
    }

    // Clear triangleItemVector
    for(QGraphicsPolygonItem *triangleItem : triangleItemVector) {
        scene->removeItem(triangleItem);
        delete triangleItem;
    }
    triangleItemVector.clear();
    triangleItemVector.squeeze();

    // Clear polygonItemVector
    for(QGraphicsPolygonItem *polygonItem : polygonItemVector) {
        scene->removeItem(polygonItem);
        delete polygonItem;
    }
    polygonItemVector.clear();
    polygonItemVector.squeeze();

    // Clear mainPathArray
    mainPathArray->clear();
    mainPathArray->shrink_to_fit();

    // Clear myPolygonBodies
    for(PolygonBody* myPolygonBody : myPolygonBodies)
    {
        delete myPolygonBody;
    }
    myPolygonBodies.clear();
    myPolygonBodies.shrink_to_fit();

    circlePath.clear();
    circlePath.shrink_to_fit();

    unsigned int executedArraySize = executedArray.size();
    Paths filteredPathArray(executedArraySize);

    std::vector<p2t::Point*>    polylines[executedArraySize];
    std::vector<p2t::Triangle*> triangles[executedArraySize];

    QVector<QPolygon> trianglePolygonVector;

    numberOfTriangles = 0;

    for(unsigned int sz = 0; sz < executedArraySize; sz++)
    {
        std::set<p2t::Point*, decltype(compPoint)> filterArray(compPoint);
        for(unsigned int cz = 0; cz < executedArray.at(sz).size(); cz++)
        {
            auto result_dup = filterArray.insert(new p2t::Point((int)executedArray.at(sz).at(cz).X,
                                                                (int)executedArray.at(sz).at(cz).Y));
            if(!result_dup.second)
            {
                numberOfThrownDuplicatePoints++;
                qDebug().noquote().nospace() << "//\\\\ DUPLICATE POINT("
                                             << executedArray.at(sz).at(cz).X << ", " << (int)executedArray.at(sz).at(cz).Y
                                             << ") THROWN //\\\\";
            }
            else
            {
                polylines[sz].push_back(new p2t::Point((int)executedArray.at(sz).at(cz).X, (int)executedArray.at(sz).at(cz).Y));
                filteredPathArray.at(sz) << IntPoint((int)executedArray.at(sz).at(cz).X, (int)executedArray.at(sz).at(cz).Y);
            }
        }

        // Clear filterArray
        for(p2t::Point* sortedPoint : filterArray)
        {
            delete sortedPoint;
        }
        filterArray.clear();

        p2t::CDT cdt(polylines[sz]);
        cdt.Triangulate();
        triangles[sz] = cdt.GetTriangles();

        b2Vec2 triangle_vertices[3];

        for (p2t::Triangle *triangle : triangles[sz])
        {
            QPolygon trianglePoly;

            for (int dz = 0; dz < 3; ++dz)
            {
                p2t::Point *p = triangle->GetPoint(dz);
                triangle_vertices[dz].x = toB2(p->x);
                triangle_vertices[dz].y = toB2(p->y);

                if(draw_triangles)
                trianglePoly.append(QPoint(p->x, p->y));

                p = nullptr;
            }

            numberOfTriangles++;

            if(draw_triangles)
                trianglePolygonVector.append(trianglePoly);

            myPolygonBodies.push_back(new PolygonBody(triangle_vertices, 3, b2Vec2(0, 0), world));

            triangle = nullptr;
        }
        triangles[sz].clear();
        triangles[sz].shrink_to_fit();

        for(p2t::Point* polylinePoint : polylines[sz])
        {
            delete polylinePoint;
        }
        polylines[sz].clear();
        polylines[sz].shrink_to_fit();
    }

    executedArray.clear();
    executedArray.shrink_to_fit();

    *mainPathArray = std::move(filteredPathArray);

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

    if(draw_triangles)
    {
        for (QPolygon trianglePolygon : trianglePolygonVector)
        {
            triangleItemVector.append(scene->addPolygon(trianglePolygon, QPen(Qt::black), QBrush(Qt::NoBrush)));
        }
    }

    auto end(std::chrono::high_resolution_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start));

    resultProcessTime = duration.count();

    if(resultProcessTime < minimumProcessTime)
        minimumProcessTime = resultProcessTime;

    if(resultProcessTime > maximumProcessTime)
        maximumProcessTime = resultProcessTime;

    // Redraw number of polygons
    numberOfPolygonsTextItem->setText(QString("Number of polygons: ") + QString::number(executedArraySize));

    // Redraw number of triangles formed from polygons
    numberOfTrianglesTextItem->setText(QString("Number of triangles formed from polygons: ") + QString::number(numberOfTriangles));

    // Redraw number of thrown duplicate points in last processing
    numberOfThrownDuplicatePointsTextItem->setText(
                QString("Number of thrown duplicate points in last processing: ") +
                QString::number(numberOfThrownDuplicatePoints));

    lastProcessDurationTextItem->setText(
                QString("Process duration: \n") + QString("Last: \n") +
                QString::number(duration.count()/1000/1000) + QString(" ms\n") +
                QString::number(duration.count()/1000) + QString(" µs\n") +
                QString::number(duration.count()) + QString(" ns\n"));

    minimumProcessDurationTextItem->setText(
                QString("\n") + QString("Minimum: \n") +
                QString::number(minimumProcessTime/1000/1000) + QString(" ms\n") +
                QString::number(minimumProcessTime/1000) + QString(" µs\n") +
                QString::number(minimumProcessTime) + QString(" ns\n"));

    maximumProcessDurationTextItem->setText(
                QString("\n") + QString("Maximum: \n") +
                QString::number(maximumProcessTime/1000/1000) + QString(" ms\n") +
                QString::number(maximumProcessTime/1000) + QString(" µs\n") +
                QString::number(maximumProcessTime) + QString(" ns\n"));

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // Accept mouse event
    event->accept();

    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseLeftKeyPressed = true;

        numberOfThrownDuplicatePoints = 0;
        numberOfTriangles = 0;
        processTheTerrain(event->pos().x(), event->pos().y());
    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
        mouseRightKeyPressed = true;
    }

    if(mouseRightKeyPressed)
    {
        coloredCirclesVector.append(new Circle(toB2(CIRCLE_RADIUS), QPointF(toB2(event->pos().x()), toB2(event->pos().y())), world));
        coloredCirclesVector.last()->setParent(scene);
        scene->addItem(coloredCirclesVector.last());
        numberOfCircles++;

        // Draw number of circle bodies
        numberOfCircleBodiesTextItem->setText(QString("Number of circle bodies: ") + QString::number(numberOfCircles));
        numberOfCircleBodiesTextItem->setPos(this->width() - 5 - numberOfCircleBodiesTextItem->boundingRect().width(), 45);

        //qDebug().noquote() << "new circle at" << toB2(event->pos().x()) << toB2(event->pos().y());
    }

    // Pass the event further
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // Accept mouse event
    event->accept();

    bool condition = true;
    if(mouseLeftKeyPressed && condition)
    {
        processTheTerrain(event->pos().x(), event->pos().y());
    }

    if(mouseRightKeyPressed)
    {
        coloredCirclesVector.append(new Circle(toB2(CIRCLE_RADIUS), QPointF(toB2(event->pos().x()), toB2(event->pos().y())), world));
        coloredCirclesVector.last()->setParent(scene);
        scene->addItem(coloredCirclesVector.last());
        numberOfCircles++;

        // Draw number of circle bodies
        numberOfCircleBodiesTextItem->setText(QString("Number of circle bodies: ") + QString::number(numberOfCircles));
        numberOfCircleBodiesTextItem->setPos(this->width() - 5 - numberOfCircleBodiesTextItem->boundingRect().width(), 45);

        //qDebug().noquote() << "new circle at" << toB2(event->pos().x()) << toB2(event->pos().y());
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
    need_delete_circles = true;
    reset();
    //connect(frameTimer, SIGNAL(timeout()), scene, SLOT(advance()));
}

void MainWindow::on_radioButtonMagenta_clicked()
{
    brush.setColor(Qt::magenta);
    pen.setColor(Qt::darkMagenta);
    repaintPolygon();
}


void MainWindow::on_radioButtonGreen_clicked()
{
    brush.setColor(Qt::green);
    pen.setColor(Qt::darkGreen);
    repaintPolygon();
}

void MainWindow::on_radioButtonRed_clicked()
{
    brush.setColor(Qt::red);
    pen.setColor(Qt::darkRed);
    repaintPolygon();
}


void MainWindow::on_radioButtonOrange_clicked()
{
    brush.setColor(QColor(255, 165, 0, 255));
    pen.setColor(QColor(238, 118, 0, 255));
    repaintPolygon();
}

void MainWindow::on_radioButtonSteelBlue_clicked()
{
    brush.setColor(QColor(176, 196, 222, 255));
    pen.setColor(QColor(70, 130, 180, 255));
    repaintPolygon();
}

void MainWindow::on_radioButtonCherry_clicked()
{
    brush.setColor(QColor(145, 30, 66, 255));
    pen.setColor(QColor(102, 21, 47, 255));
    repaintPolygon();
}

void MainWindow::on_radioButtonWhite_clicked()
{
    brush.setColor(QColor(255, 255, 255, 255));
    pen.setColor(QColor(128, 128, 128, 255));
    repaintPolygon();
}

void MainWindow::on_radioButtonGrey_clicked()
{
    brush.setColor(QColor(128, 128, 128, 255));
    pen.setColor(QColor(102, 102, 102, 255));
    repaintPolygon();
}

void MainWindow::on_radioButtonYes_clicked()
{
    draw_triangles = true;
}

void MainWindow::on_radioButtonNo_clicked()
{
    draw_triangles = false;
}

Scene::Scene(qreal x, qreal y, qreal width, qreal height, b2World *world)
    : QGraphicsScene(fromB2(x), fromB2(y), fromB2(width), fromB2(height))
{
    this->world = world;
}

Scene::~Scene()
{
    if(need_delete_circles && numberOfCircles > 0)
    {
        for(Circle *circle : coloredCirclesVector)
        {
            numberOfCircles--;
            this->removeItem(circle);
            //delete circle;
        }
        need_delete_circles = false;

        coloredCirclesVector.clear();
        coloredCirclesVector.squeeze();
    }
}

void Scene::advance()
{
    if(numberOfCircles > 0)
    {
        for(Circle *circle : coloredCirclesVector)
        {
            if(circle->needDelete)
            {
                this->removeItem(circle);
                coloredCirclesVector.removeOne(circle);
                //delete circle;

                numberOfCircles--;

                // Draw number of circle bodies
                numberOfCircleBodiesTextItem->setText(QString("Number of circle bodies: ") + QString::number(numberOfCircles));
                numberOfCircleBodiesTextItem->setPos(this->width() - 5 - numberOfCircleBodiesTextItem->boundingRect().width(), 45);
                //qDebug().noquote() << " circle deleted";
            }
            else if(need_delete_circles)
            {
                this->removeItem(circle);
                //delete circle;

                numberOfCircles = 0;
            }
        }
        if(need_delete_circles)
        {
            coloredCirclesVector.clear();
            need_delete_circles = false;
        }
        coloredCirclesVector.squeeze();
    }
    world->Step(timeStep, velocityIterations, positionIterations);

    //qDebug().noquote() << "scene items:" << this->items().count();

    QGraphicsScene::advance();
}

Circle::Circle(qreal radius, QPointF initPos, b2World *world)
    : QGraphicsEllipseItem(0)
{
    QPen circlePen;
    circlePen.setColor(Qt::black);
    //circlePen.setStyle(Qt::DotLine);
    setPen(circlePen);

    int color = qrand() % ((10 + 1) - 1) + 1; // random min 1, max 10
    switch(color)
    {
    case 1:
        setBrush(QBrush(Qt::red));
        break;
    case 2:
        setBrush(QBrush(Qt::green));
        break;
    case 3:
        setBrush(QBrush(Qt::blue));
        break;
    case 4:
        setBrush(QBrush(QColor(255, 192, 203)));    // Pink color
        break;
    case 5:
        setBrush(QBrush(QColor(145, 30, 66)));      // Cherry color
        break;
    case 6:
        setBrush(QBrush(QColor(51, 100, 100)));     // Gold color
        break;
    case 7:
        setBrush(QBrush(QColor(0, 0, 75)));         // Silver color
        break;
    case 8:
        setBrush(QBrush(Qt::gray));                 // Gray color
        break;
    case 9:
        setBrush(QBrush(Qt::black));                // Black color
        break;
    case 10:
        setBrush(QBrush(Qt::white));                // White color
        break;
    default:
        setBrush(QBrush(Qt::cyan));
        break;
    }

    setRect(-fromB2(radius), -fromB2(radius), fromB2(radius*2), fromB2(radius*2));
    setPos(fromB2(initPos.x()), fromB2(initPos.y()));

    b2BodyDef heroCircleBodyDef;
    heroCircleBodyDef.type = b2_dynamicBody;
    heroCircleBodyDef.position = b2Vec2(initPos.x(), initPos.y());
    heroCircleBodyDef.linearDamping = 0.5f;

    body = world->CreateBody(&heroCircleBodyDef);

    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef FixtureDef;
    FixtureDef.density = 0.2f;
    FixtureDef.friction = 0.2f;
    FixtureDef.restitution = 0.7f;
    FixtureDef.shape = &circle;

    body->CreateFixture(&FixtureDef);
    body->SetUserData(this);

}

Circle::~Circle()
{
    //this->scene()->removeItem(this);
    body->GetWorld()->DestroyBody(body);
}

void Circle::advance(int phase)
{
    if(phase)
    {
        if(this->y() > 480)
        {
            this->needDelete = true;
        }
        else
        {
            setPos(fromB2(body->GetPosition().x), fromB2(body->GetPosition().y));
        }
    }
}










