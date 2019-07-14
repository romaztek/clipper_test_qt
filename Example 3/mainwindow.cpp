#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BRUSH_WIDTH 30
#define CIRCLE_RADIUS 6

qreal fromB2(qreal value) {
    return value*SCALE;
}

qreal toB2(qreal value) {
    return value/SCALE;
}

QVector<PolygonBody*> myPolygonBodies;

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

    this->move(710, 60);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setFixedSize(this->width(), this->height());

    ui->horizontalSlider->setValue(BRUSH_WIDTH);

    b2Vec2 gravity(0.0f, 10.0f);
    world = new b2World(gravity);

    scene = new Scene(0, 0, toB2(640), toB2(480), world);

    QGraphicsView *view = this->findChild<QGraphicsView*>("graphicsView");
    view->setScene(scene);

    view->viewport()->setFocus(Qt::MouseFocusReason);
    view->viewport()->setAttribute(Qt::WA_TransparentForMouseEvents);

    mainPath = new Paths(1);
    mainPath->at(0) << IntPoint(40, 200) << IntPoint(600, 200) <<
                   IntPoint(600, 420) << IntPoint(40, 420);

    QGraphicsRectItem *groundRect = new QGraphicsRectItem(0, this->height() - 80, this->width(),  20);
    groundRect->setPen(QPen(Qt::black));
    groundRect->setBrush(QBrush(Qt::yellow));

    const float bottomGroundX_B2 = toB2(this->width()/2);
    const float bottomGroundY_B2 = toB2(this->height() - 80 + 20/2);
    const float bottomGroundWidth_B2 = toB2(640);
    const float bottomGroundHeight_B2 = toB2(20);

    Wall *bottomGround = new Wall(bottomGroundX_B2, bottomGroundY_B2,
                                  bottomGroundWidth_B2, bottomGroundHeight_B2, 10, world);

    b2Vec2 myPolygonBodyVertices[mainPath->at(0).size()];

    scene->addItem(groundRect);

    QPolygon mainPath_poly;

    for(unsigned int i = 0; i < mainPath->at(0).size(); i++)
    {
        mainPath_poly.append(QPoint((int)mainPath->at(0).at(i).X, (int)mainPath->at(0).at(i).Y));

        myPolygonBodyVertices[i].x = toB2((int)mainPath->at(0).at(i).X);
        myPolygonBodyVertices[i].y = toB2((int)mainPath->at(0).at(i).Y);

        /*qDebug().noquote() << myPolygonBodyVertices[i].x << myPolygonBodyVertices[i].y
                           << (int)mainPath->at(0).at(i).X << (int)mainPath->at(0).at(i).Y;*/
    }

    myPolygonBodies.append(new PolygonBody(myPolygonBodyVertices, 4, b2Vec2(0, 0), world));

    pen.setCosmetic(true);
    pen.setColor(Qt::darkMagenta);
    pen.setWidth(2);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::magenta);

    polygonItem.append(scene->addPolygon(mainPath_poly, pen, brush));

    numberOfPolygonsTextItem = scene->addText(QString("Number of polygons: ") + QString::number(1));
    numberOfPolygonsTextItem->setPos(5, 5);

    numberOfThrownDuplicateVerticesTextItem = scene->addText(QString("Number of thrown duplicate vertices in last processing: ")
                                                             + QString::number(numberOfThrownDuplicateVertices));
    numberOfThrownDuplicateVerticesTextItem->setPos(5, 25);

    frameTimer = new QTimer(this);
    connect(frameTimer, SIGNAL(timeout()), scene, SLOT(advance()));
    frameTimer->start(1000/60);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCirclePath(int _x, int _y, int _r, Paths *path)        // Create a circle polygon Path
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
    mainPath->at(0) << IntPoint(40, 200) << IntPoint(600, 200) <<
                   IntPoint(600, 420) << IntPoint(40, 420);

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

    scene->removeItem(numberOfPolygonsTextItem);
    numberOfPolygonsTextItem = scene->addText(QString("Number of polygons: ") + QString::number(mainPath->size()));
    numberOfPolygonsTextItem->setPos(5, 5);


    scene->removeItem(numberOfThrownDuplicateVerticesTextItem);
    numberOfThrownDuplicateVertices = 0;
    numberOfThrownDuplicateVerticesTextItem = scene->addText(QString("Number of thrown duplicate vertices in last processing: ")
                                                             + QString::number(numberOfThrownDuplicateVertices));
    numberOfThrownDuplicateVerticesTextItem->setPos(5, 25);

    qDebug().noquote() << "Terrain reseted:" << reseted;

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

        // Draw all polygons after clipping
        for(unsigned int i = 0; i < mainPath->size(); i++)
        {
            polygonItem.append(scene->addPolygon(solution_poly[i], pen, brush));
        }
    }
    busy = false;
}

void MainWindow::processTheTerrain(int mouse_x, int mouse_y)
{
    Paths circlePath(1);    // Paths(1) for circle's path
    Paths proceededPath;    // Paths for proceeded polygon after clipping

    // Create clipper circle's path, position based on mouse position
    setCirclePath(mouse_x, mouse_y, brush_width, &circlePath);

    // Clipping
    Clipper c;
    c.AddPaths(*mainPath, ptSubject, true);
    c.AddPaths(circlePath, ptClip, true);
    c.Execute(ctDifference, proceededPath, pftEvenOdd , pftEvenOdd);

    //*mainPath = proceededPath;       // After clipping: main polygon = proceededPath (current polygon)

    foreach (PolygonBody *tmp_poly, myPolygonBodies) {
        tmp_poly->body->GetWorld()->DestroyBody(tmp_poly->body);
    }

    size_t proceededPathSize = proceededPath.size();

    Paths filteredPath(proceededPathSize);

    // Polygons array for drawing
    QPolygon solution_poly[proceededPathSize];

    // Add points from Paths to QPolygon polygons
    for(unsigned int i = 0; i < proceededPathSize; i++)
    {
        for(unsigned int j = 0; j < proceededPath.at(i).size(); j++)
        {
            solution_poly[i].append(QPoint((int)proceededPath[i][j].X, (int)proceededPath[i][j].Y));
        }
    }

    myPolygonBodies.clear();

    std::vector<p2t::Point*>    polylines[proceededPathSize];
    std::vector<p2t::Triangle*> triangles[proceededPathSize];

    for(unsigned int sz = 0; sz < proceededPathSize; sz++)
    {
        std::set<p2t::Point*, decltype(compPoint)> sorted_lines(compPoint);

        for(unsigned int cz = 0; cz < proceededPath[sz].size(); cz++)
        {
            auto result_dup = sorted_lines.insert(new p2t::Point((int)proceededPath[sz][cz].X, (int)proceededPath[sz][cz].Y));

            if(!result_dup.second)
            {
                numberOfThrownDuplicateVertices++;
                qDebug().noquote() << "//\\\\ DUPLICATE"
                                   << proceededPath[sz][cz].X << (int)proceededPath[sz][cz].Y
                                   << "THROWN //\\\\";
            }
            else
            {
                polylines[sz].push_back(new p2t::Point((int)proceededPath[sz][cz].X, (int)proceededPath[sz][cz].Y));
                filteredPath.at(sz) << IntPoint((int)proceededPath[sz][cz].X, (int)proceededPath[sz][cz].Y);
            }
        }

        //mainPath->at(sz).push_back(polylines[sz]);

        p2t::CDT cdt(polylines[sz]);
        cdt.Triangulate();
        triangles[sz] = cdt.GetTriangles();

        b2Vec2 triangle_vertices[3];

        for (p2t::Triangle *tri : triangles[sz])
        {
            for (int i = 0; i < 3; ++i)
            {
             const p2t::Point &p = *tri->GetPoint(i);
             triangle_vertices[i].x = toB2(p.x);
             triangle_vertices[i].y = toB2(p.y);

             //qDebug().noquote() << "triangle" << i << p.x << p.y;
             //triangle_test_poly.append(QPoint(triangle_vertices[i].x, triangle_vertices[i].y));
            }
            myPolygonBodies.append(new PolygonBody(triangle_vertices, 3, b2Vec2(0, 0), world));
        }
    }

    mainPath->clear();
    *mainPath = filteredPath;

    foreach (QGraphicsPolygonItem *tmp_poly, polygonItem) {
        scene->removeItem(tmp_poly);
    }

    polygonItem.clear();

    // Draw all polygons after clipping
    for(unsigned int i = 0; i < proceededPathSize; i++)
    {
        polygonItem.append(scene->addPolygon(solution_poly[i], pen, brush));
    }

    // Remove and redraw number of polygons
    scene->removeItem(numberOfPolygonsTextItem);
    numberOfPolygonsTextItem = scene->addText(QString("Number of polygons: ") + QString::number(proceededPathSize));
    numberOfPolygonsTextItem->setPos(5, 5);

    // Remove and redraw number of thrown duplicate vertices in last processing
    scene->removeItem(numberOfThrownDuplicateVerticesTextItem);
    numberOfThrownDuplicateVerticesTextItem = scene->addText(QString("Number of thrown duplicate vertices in last processing: ")
                                                             + QString::number(numberOfThrownDuplicateVertices));
    numberOfThrownDuplicateVerticesTextItem->setPos(5, 25);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();                    // Accept mouse event
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseLeftKeyPressed = true;
        reseted = false;

        if(!busy)
        {
            numberOfThrownDuplicateVertices = 0;
            processTheTerrain(event->pos().x(), event->pos().y());
        }

    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
        mouseRightKeyPressed = true;
    }

    if(mouseRightKeyPressed  && !busy) {
        scene->addItem(new Circle(toB2(CIRCLE_RADIUS), QPointF(toB2(event->pos().x()), toB2(event->pos().y())), world));
        qDebug().noquote() << "new circle at" << toB2(event->pos().x()) << toB2(event->pos().y());
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    bool condition = true;
    if(mouseLeftKeyPressed && !busy && condition) {
        processTheTerrain(event->pos().x(), event->pos().y());
    }

    if(mouseRightKeyPressed && !busy) {
        scene->addItem(new Circle(toB2(CIRCLE_RADIUS), QPointF(toB2(event->pos().x()), toB2(event->pos().y())), world));

        //qDebug().noquote() << "new circle at" << toB2(event->pos().x()) << toB2(event->pos().y());
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

void MainWindow::on_deleteCirclesButton_clicked()
{
    qDebug().noquote() << "yep";
    b2Body * B = world->GetBodyList();
    while(B != NULL)
    {
        b2Fixture* F = B->GetFixtureList();
        while(F != NULL)
        {
            switch (F->GetType())
            {
            case b2Shape::e_circle:
            {
                qDebug().noquote() << "yes";
                //b2CircleShape* circle = (b2CircleShape*) F->GetShape();
                //Circle *m_circle = reinterpret_cast<Circle*>(F->GetUserData());
                /* Do stuff with a circle shape */
            }
                break;

            case b2Shape::e_polygon:
            {
                b2PolygonShape* poly = (b2PolygonShape*) F->GetShape();
                /* Do stuff with a polygon shape */
            }
                break;
            }
            F = F->GetNext();
        }

        B = B->GetNext();
    }
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

void MainWindow::on_radioButtonCherry_toggled(bool checked)
{
    if(checked) {
        brush.setColor(QColor(145, 30, 66, 255));
        pen.setColor(QColor(102, 21, 47, 255));
    }
    repaintPolygon();
}

void MainWindow::on_radioButtonWhite_toggled(bool checked)
{
    if(checked) {
        brush.setColor(QColor(255, 255, 255, 255));
        pen.setColor(QColor(128, 128, 128, 255));
    }
    repaintPolygon();
}

void MainWindow::on_radioButtonGrey_toggled(bool checked)
{
    if(checked) {
        brush.setColor(QColor(128, 128, 128, 255));
        pen.setColor(QColor(102, 102, 102, 255));
    }
    repaintPolygon();
}

Scene::Scene(qreal x, qreal y, qreal width, qreal height, b2World *world)
    : QGraphicsScene(fromB2(x), fromB2(y), fromB2(width), fromB2(height))
{
    this->world = world;
}

void Scene::advance()
{
    world->Step(timeStep, velocityIterations, positionIterations);

    //qDebug().noquote() << "Scene advance";

    QGraphicsScene::advance();
}

Circle::Circle(qreal radius, QPointF initPos, b2World *world)
    : QGraphicsEllipseItem(0)
{
    //setBrush(QBrush(Qt::green));
    setPen(QPen(Qt::black));

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
    body->GetWorld()->DestroyBody(body);
}

void Circle::advance(int phase)
{
    if(phase)
    {
        setPos(fromB2(body->GetPosition().x), fromB2(body->GetPosition().y));

        if(this->y() > 480)
        {
            qDebug().noquote() << "circle deleted";
            delete this;
        }
    }
}







