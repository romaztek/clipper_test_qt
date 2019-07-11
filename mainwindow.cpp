#include "mainwindow.h"
#include "ui_mainwindow.h"

Paths my_subj(1);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QBrush brush_1;
    brush_1.setStyle(Qt::SolidPattern);
    brush_1.setColor(QColor(255, 0, 0, 128));

    QBrush brush_2;
    brush_2.setStyle(Qt::SolidPattern);
    brush_2.setColor(QColor(0, 255, 0, 128));

    QPen pen(Qt::black);
    //this->setGeometry(0, 0, 640, 480);
    ui->setupUi(this);
    scene = new MyScene(0, 0, 640, 480);

    connect(scene, SIGNAL(sendPos(int,int)), this, SLOT(make_it(int,int)));

    QGraphicsView *view = this->findChild<QGraphicsView*>("graphicsView");
    view->setScene(scene);

    mainPath = new Paths(1);

    mainPath->at(0) << IntPoint(100, 100) << IntPoint(500, 100) <<
                   IntPoint(500, 400) << IntPoint(100, 400);

    QPolygon mainPath_poly;
    for(unsigned int i = 0; i < mainPath->at(0).size(); i++) {
        mainPath_poly.append(QPoint((int)mainPath->at(0).at(i).X, (int)mainPath->at(0).at(i).Y));
    }

    scene->addPolygon(mainPath_poly, Qt::NoPen, QBrush(Qt::magenta));

    /*Paths my_subj(1), my_clip(1), my_solution;

    my_subj[0] <<
        IntPoint(100, 100) << IntPoint(400, 100) <<
        IntPoint(400, 400) << IntPoint(100, 400);

    QPolygon my_subj_poly;
    for(unsigned int i = 0; i < my_subj.at(0).size(); i++) {
        my_subj_poly.append(QPoint((int)my_subj[0][i].X, (int)my_subj[0][i].Y));
    }

    my_clip[0] <<
        IntPoint(200, 200) << IntPoint(600, 200) <<
        IntPoint(600, 300) << IntPoint(200, 300);

    QPolygon my_clip_poly;
    for(unsigned int i = 0; i < my_clip.at(0).size(); i++) {
        my_clip_poly.append(QPoint((int)my_clip[0][i].X, (int)my_clip[0][i].Y));
    }


    QGraphicsEllipseItem ellipse(300, 300, 60, 60);

    QPolygon ellipse_poly;




    Clipper c;
    c.AddPaths(my_subj, ptSubject, true);
    c.AddPaths(my_clip, ptClip, true);
    qDebug().noquote() << "if" << c.Execute(ctDifference, my_solution, pftEvenOdd , pftEvenOdd);

    QPolygon my_sol_poly;

    for(unsigned int i = 0; i < my_solution.size(); i++) {
        for(unsigned int j = 0; j < my_solution[i].size(); j++) {
            my_sol_poly.append(QPoint((int)my_solution[i][j].X, (int)my_solution[i][j].Y));
            //qDebug().noquote() << i << (int)my_solution[i][j].X << (int)my_solution[i][j].Y;

            //my_subj[0] << IntPoint(my_solution[0][i].X, my_solution[0][i].Y);
        }
    }
    qDebug().noquote() << my_solution.size();

    //scene->addPolygon(my_subj_poly, pen, brush_1);
    //scene->addPolygon(my_clip_poly, pen, brush_2);

    scene->addPolygon(my_sol_poly, Qt::NoPen, QBrush(Qt::magenta));

    QPolygon test_poly;
    Paths circlePath(1), new_solution;

    Circle(100, 100, 60, &circlePath);

    for(unsigned int i = 0; i < circlePath.at(0).size(); i++) {
        test_poly.append(QPoint((int)circlePath[0][i].X, (int)circlePath[0][i].Y));
    }

    scene->addPolygon(test_poly, pen, QBrush(QColor(255, 0, 0, 128)));

    Clipper c2;
    c2.AddPaths(my_solution, ptSubject, true);
    c2.AddPaths(circlePath, ptClip, true);
    qDebug().noquote() << "if_2" << c2.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);


    QPolygon my_new_sol_poly;

    for(unsigned int i = 0; i < new_solution.size(); i++) {
        for(unsigned int j = 0; j < new_solution[i].size(); j++) {
            my_new_sol_poly.append(QPoint((int)new_solution[i][j].X, (int)new_solution[i][j].Y));
            //qDebug().noquote() << i << (int)new_solution[i][j].X << (int)new_solution[i][j].Y;
        }
    }
    qDebug().noquote() << my_new_sol_poly.size();
    qDebug().noquote() << new_solution.size();

    scene->addPolygon(my_new_sol_poly, Qt::NoPen, QBrush(Qt::green));


    //scene->addPolygon(my_sol_poly, Qt::NoPen, QBrush(QColor(255, 255, 255, 128)));*/

    /*QPolygon my_end_poly;
    for(unsigned int i = 0; i < my_subj.at(0).size(); i++) {
        my_end_poly.append(QPoint((int)my_subj[0][i].X, (int)my_subj[0][i].Y));


    //scene->addPolygon(my_end_poly, pen, QBrush(QColor(0, 0, 0, 128)));

    Paths subj(2), clip(1), solution;

    //define outer blue 'subject' polygon
    subj[0] <<
      IntPoint(180,200) << IntPoint(260,200) <<
      IntPoint(260,150) << IntPoint(180,150);

    //define subject's inner triangular 'hole' (with reverse orientation)
    subj[1] <<
      IntPoint(215,120) << IntPoint(230,190) << IntPoint(200,190);

    //define orange 'clipping' polygon
    clip[0] <<
      IntPoint(190,210) << IntPoint(240,210) <<
      IntPoint(240,130) << IntPoint(190,130);



    //draw input polygons with user-defined routine ...
    QPolygon pol_1_1;
    pol_1_1.append(QPoint((int)subj[0][0].X, (int)subj[0][0].Y));
    pol_1_1.append(QPoint((int)subj[0][1].X, (int)subj[0][1].Y));
    pol_1_1.append(QPoint((int)subj[0][2].X, (int)subj[0][2].Y));
    pol_1_1.append(QPoint((int)subj[0][3].X, (int)subj[0][3].Y));

    QPolygon pol_1_2;
    pol_1_2.append(QPoint((int)subj[1][0].X, (int)subj[1][0].Y));
    pol_1_2.append(QPoint((int)subj[1][1].X, (int)subj[1][1].Y));
    pol_1_2.append(QPoint((int)subj[1][2].X, (int)subj[1][2].Y));

    QPolygon pol_2;
    pol_2.append(QPoint((int)clip[0][0].X, (int)clip[0][0].Y));
    pol_2.append(QPoint((int)clip[0][1].X, (int)clip[0][1].Y));
    pol_2.append(QPoint((int)clip[0][2].X, (int)clip[0][2].Y));
    pol_2.append(QPoint((int)clip[0][3].X, (int)clip[0][3].Y));

    //DrawPolygons(subj, 0x160000FF, 0x600000FF); //blue
    //DrawPolygons(clip, 0x20FFFF00, 0x30FF0000); //orange

    QPolygon pol_1_sub;
    pol_1_sub = pol_1_1.subtracted(pol_1_2);


    scene->addPolygon(pol_1_sub, Qt::NoPen, brush_1);
    //scene->addPolygon(pol_1_2, pen, brush_1);
    scene->addPolygon(pol_2, pen, brush_2);

    //perform intersection ...
    Clipper c;
    c.AddPaths(subj, ptSubject, false);
    c.AddPaths(clip, ptClip, true);
    qDebug().noquote() << "if" << c.Execute(ctDifference, solution, pftNonZero, pftNonZero);

    //draw solution with user-defined routine ...

    QPolygon pol_s[solution.size()];
    QPolygon pol_s_sub;
   // pol_s.append(QPoint((int)solution[0][i], (int)solution[0][i]));

    for(int i = 0; i < solution.size(); i++) {
        for(int j = 0; j < solution.at(i).size(); j++) {
            qDebug().noquote() << solution[i][j].X << solution[i][j].Y;
            pol_s[i].append(QPoint((int)solution[i][j].X, (int)solution[i][j].Y));
        }
    }
    qDebug().noquote() << solution.size();
    qDebug().noquote() << solution.at(0).size();
    qDebug().noquote() << solution.at(1).size();

    pol_s_sub = pol_s[0].subtracted(pol_s[1]);


    scene->addPolygon(pol_s[1], Qt::NoPen, QBrush(QColor(0, 0, 255, 255), Qt::SolidPattern));*/

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

        //poly->append(QPoint(x1, y1));

        //PutPixel(x1, y1, 15);

        n = n + dn;
    }
}

void MainWindow::make_it(int g_x, int g_y)
{
    /*qDebug().noquote() << g_x << g_y;

    QPolygon solution_poly;
    Paths circlePath(1), new_solution;

    Circle(g_x, g_y, 40, &circlePath);

    Clipper c;
    c.AddPaths(*mainPath, ptSubject, true);
    c.AddPaths(circlePath, ptClip, true);
    c.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);

    for(unsigned int i = 0; i < new_solution.at(0).size(); i++) {
        solution_poly.append(QPoint((int)new_solution[0][i].X, (int)new_solution[0][i].Y));
    }

    scene->clear();
    scene->addPolygon(solution_poly, Qt::NoPen, QBrush(Qt::magenta));*/
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{

    event->accept();
    int g_x = event->pos().x();
    int g_y = event->pos().y();
    qDebug().noquote() << g_x << g_y;

    QPolygon solution_poly;
    Paths circlePath(1), new_solution;


    Circle(g_x, g_y, 40, &circlePath);

    Clipper c;
    c.AddPaths(*mainPath, ptSubject, true);
    c.AddPaths(circlePath, ptClip, true);
    c.Execute(ctDifference, new_solution, pftEvenOdd , pftEvenOdd);

    *mainPath = new_solution;

    for(unsigned int i = 0; i < new_solution.at(0).size(); i++) {
        solution_poly.append(QPoint((int)new_solution[0][i].X, (int)new_solution[0][i].Y));
    }

    scene->clear();
    scene->addPolygon(solution_poly, Qt::NoPen, QBrush(Qt::magenta));

    scene->addEllipse(g_x - 5, g_y - 5, 10, 10, QPen(Qt::black), QBrush(Qt::gray));
}
