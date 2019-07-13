#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BRUSH_WIDTH 30
#define CIRCLE_RADIUS 4

qreal fromB2(qreal value) {
    return value*SCALE;
}

qreal toB2(qreal value) {
    return value/SCALE;
}

QVector<PolygonBody*> myPolygonBodies;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    b2Vec2 ground_vertices[4];
    b2Vec2 myPolygonBodyVertices[mainPath->at(0).size()];

    scene->addItem(groundRect);

    QPolygonF testPolygon;

    QPolygon mainPath_poly;
    for(unsigned int i = 0; i < mainPath->at(0).size(); i++) {
        mainPath_poly.append(QPoint((int)mainPath->at(0).at(i).X, (int)mainPath->at(0).at(i).Y));

        myPolygonBodyVertices[i].x = toB2((int)mainPath->at(0).at(i).X);
        myPolygonBodyVertices[i].y = toB2((int)mainPath->at(0).at(i).Y);

        testPolygon.append(QPoint(fromB2(myPolygonBodyVertices[i].x), fromB2(myPolygonBodyVertices[i].y)));

        qDebug().noquote() << myPolygonBodyVertices[i].x << myPolygonBodyVertices[i].y
                           << (int)mainPath->at(0).at(i).X << (int)mainPath->at(0).at(i).Y;
    }

    myPolygonBodies.append(new PolygonBody(myPolygonBodyVertices, 4,
                                           b2Vec2(0, 0), world));

    //PolygonBody *myPolygonBody =

    pen.setCosmetic(true);
    pen.setColor(Qt::darkMagenta);
    pen.setWidth(2);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::magenta);

    polygonItem.append(scene->addPolygon(mainPath_poly, pen, brush));


    //scene->addPolygon(testPolygon, QPen(Qt::black), QBrush(Qt::yellow));

    textItem = scene->addText(QString("Number of polygons: ") + QString::number(1));
    textItem->setPos(5, 5);

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

    scene->removeItem(textItem);

    textItem = scene->addText(QString("Number of polygons: ") + QString::number(mainPath->size()));
    textItem->setPos(5, 5);

    qDebug().noquote() << "reseted" << reseted;

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
        qDebug().noquote() << "Mouse x/y:" << toB2(mouse_x) << toB2(mouse_y);

        Paths circlePath(1);    // Paths for circle's path
        Paths new_solution;     // Paths for proceeded polygon after clipping

        setCirclePath(mouse_x, mouse_y, brush_width, &circlePath);  // Clipping circle, position based on mouse position

        // Clipping
        Clipper c;
        c.AddPaths(*mainPath, ptSubject, true);
        c.AddPaths(circlePath, ptClip, true);
        c.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);

        *mainPath = new_solution;       // After clipping: clipped polygon = main polygon (current polygon)


        busy = true;

        foreach (PolygonBody *tmp_poly, myPolygonBodies) {
            tmp_poly->body->GetWorld()->DestroyBody(tmp_poly->body);
        }

        myPolygonBodies.clear();

        // Polygons array for drawing
        QPolygon solution_poly[new_solution.size()];

        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            //b2Vec2 myPolygonBodyVertices[new_solution[i].size()];
            for(unsigned int j = 0; j < new_solution[i].size(); j++)
            {
                // Add points from Paths to polygons
                solution_poly[i].append(QPoint((int)new_solution[i][j].X, (int)new_solution[i][j].Y));
            }
        }

        // 222222222222222222222222222222222222222222222222222222222222222222222222

        std::vector<p2t::Point*> polylines[new_solution.size()];
        std::vector<p2t::Triangle*> triangles[new_solution.size()];

        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            for(unsigned int j = 0; j < new_solution[i].size(); j++)
            {
                polylines[i].push_back(new p2t::Point((int)new_solution[i][j].X, (int)new_solution[i][j].Y));
            }
        }

        for(unsigned int sz = 0; sz < new_solution.size(); sz++)
        {
             p2t::CDT cdt(polylines[sz]);
             cdt.Triangulate();
             triangles[sz] = cdt.GetTriangles();

             b2Vec2 triangle_vertices[3];

             for (p2t::Triangle *tri : triangles[sz])
             {
                 for (int i = 0; i < 3; ++i)
                 {
                     const p2t::Point &p = *tri->GetPoint(i);
                     triangle_vertices[i].x = toB2(p.x) + 0.001f;
                     triangle_vertices[i].y = toB2(p.y) + 0.001f;

                     //qDebug().noquote() << "triangle" << i << p.x << p.y;
                     //triangle_test_poly.append(QPoint(triangle_vertices[i].x, triangle_vertices[i].y));
                 }
                 myPolygonBodies.append(new PolygonBody(triangle_vertices, 3,
                                                                    b2Vec2(0, 0), world));
                 //polygonItem.append(scene->addPolygon(triangle_test_poly, pen, QBrush(Qt::gray)));

             }
        }


        // 222222222222222222222222222222222222222222222222222222222222222222222222

        /*
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        std::vector<p2t::Point*> polyline;
        std::vector<p2t::Triangle*> triangles;

        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            for(unsigned int j = 0; j < new_solution[0].size(); j++) {
                polyline.push_back(new p2t::Point((int)new_solution[0][j].X, (int)new_solution[0][j].Y));
            }
        }
        //for(unsigned int i = 0; i < new_solution.size(); i++)
        //{
        //if(polyline.size() >= 3) {
            p2t::CDT cdt(polyline);
            cdt.Triangulate();
            triangles = cdt.GetTriangles();

            /// The polygon shape's vertices (actually a triangle)
            b2Vec2 triangle_vertices[3];

            /// Iterate over all triangles

            for (p2t::Triangle *tri : triangles)
            {
                for (int i = 0; i < 3; ++i)
                {
                    const p2t::Point &p = *tri->GetPoint(i);
                    triangle_vertices[i].x = toB2(p.x) + 0.001f;
                    triangle_vertices[i].y = toB2(p.y) + 0.001f;

                    //qDebug().noquote() << "triangle" << i << p.x << p.y;
                    //triangle_test_poly.append(QPoint(triangle_vertices[i].x, triangle_vertices[i].y));
                }
                myPolygonBodies.append(new PolygonBody(triangle_vertices, 3,
                                                                   b2Vec2(0, 0), world));
                //polygonItem.append(scene->addPolygon(triangle_test_poly, pen, QBrush(Qt::gray)));

            }
        */

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        busy = false;

        foreach (QGraphicsPolygonItem *tmp_poly, polygonItem) {
            scene->removeItem(tmp_poly);
        }
        polygonItem.clear();

        // Draw all polygons after clipping
        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            polygonItem.append(scene->addPolygon(solution_poly[i], pen, brush));
        }



        // Remove and redraw number of polygons
        scene->removeItem(textItem);
        textItem = scene->addText(QString("Number of polygons: ") + QString::number(new_solution.size()));
        textItem->setPos(5, 5);


        //qDebug().noquote() << new_solution.size() << "polygons";  // Print number of polygons to console

        // Uncomment if you want to see a position of last mouse press
        //scene->addEllipse(mouse_x - 5, mouse_y - 5, 10, 10, QPen(Qt::black), QBrush(Qt::gray));

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
        int mouse_x = event->pos().x();     // Store mouse X
        int mouse_y = event->pos().y();     // Store mouse Y

        // Print mouse coordinates to console
        //qDebug().noquote() << "Mouse x/y:" << mouse_x << mouse_y;

        Paths circlePath(1);    // Paths for circle's path
        Paths new_solution;     // Paths for proceeded polygon after clipping

        setCirclePath(mouse_x, mouse_y, brush_width, &circlePath);  // Clipping circle, position based on mouse position

        // Clipping
        Clipper c;
        c.AddPaths(*mainPath, ptSubject, true);
        c.AddPaths(circlePath, ptClip, true);
        c.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);

        *mainPath = new_solution;       // After clipping: clipped polygon = main polygon (current polygon)

        foreach (PolygonBody *tmp_poly, myPolygonBodies) {
            tmp_poly->body->GetWorld()->DestroyBody(tmp_poly->body);
        }

        myPolygonBodies.clear();

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


        // 222222222222222222222222222222222222222222222222222222222222222222222222

        std::vector<p2t::Point*> polylines[new_solution.size()];
        std::vector<p2t::Triangle*> triangles[new_solution.size()];

        for(unsigned int i = 0; i < new_solution.size(); i++)
        {
            for(unsigned int j = 0; j < new_solution[i].size(); j++)
            {
                polylines[i].push_back(new p2t::Point((int)new_solution[i][j].X, (int)new_solution[i][j].Y));
            }
        }

        for(unsigned int sz = 0; sz < new_solution.size(); sz++)
        {
             p2t::CDT cdt(polylines[sz]);
             cdt.Triangulate();
             triangles[sz] = cdt.GetTriangles();

             b2Vec2 triangle_vertices[3];

             for (p2t::Triangle *tri : triangles[sz])
             {
                 for (int i = 0; i < 3; ++i)
                 {
                     const p2t::Point &p = *tri->GetPoint(i);
                     triangle_vertices[i].x = toB2(p.x) + 0.001f;
                     triangle_vertices[i].y = toB2(p.y) + 0.001f;

                     //qDebug().noquote() << "triangle" << i << p.x << p.y;
                     //triangle_test_poly.append(QPoint(triangle_vertices[i].x, triangle_vertices[i].y));
                 }
                 myPolygonBodies.append(new PolygonBody(triangle_vertices, 3,
                                                                    b2Vec2(0, 0), world));
                 //polygonItem.append(scene->addPolygon(triangle_test_poly, pen, QBrush(Qt::gray)));

             }
        }


        // 222222222222222222222222222222222222222222222222222222222222222222222222


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

        //qDebug().noquote() << new_solution.size() << "polygons";  // Print number of polygons to console
    }

    if(mouseRightKeyPressed && !busy) {
        scene->addItem(new Circle(toB2(CIRCLE_RADIUS), QPointF(toB2(event->pos().x()), toB2(event->pos().y())), world));

        qDebug().noquote() << "ew circle at" << toB2(event->pos().x()) << toB2(event->pos().y());
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

    int color = qrand() % 4;
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
    default:
        setBrush(QBrush(Qt::red));
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
    if(phase) {
        setPos(fromB2(body->GetPosition().x), fromB2(body->GetPosition().y));
    }
}




