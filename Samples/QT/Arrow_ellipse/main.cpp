#include "mainwindow.h"
#include <QApplication>
#include <qdebug.h>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QProgressBar>
//#include <QtWidgets>
int x = 0;
int y = 0;
//QProgressBar *bar[6];
QTableWidget *ptable;

int x1, x2, x3, x4, x5, x6;
int y_1, y2, y3, y4, y5, y6;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDesktopWidget dw;
    x=dw.width();
    y=dw.height();
    int x_offset=200;
    x1 = x/2+x_offset;
    x2 = (x/4*3)+x_offset-40;
    x3 = (x/2)+x_offset;
    x4 = x/4+x_offset+40;
    x5 = (x/4*3)+x_offset-40;
    x6 = (x/4)+x_offset+40;

    y_1 = 100;
    y2 = (y/4);
    y3 = ((y/4)*3)-50;
    y4 = (y/4);
    y5 = ((y/4)*3)-200;
    y6 = ((y/4)*3)-200;

    qDebug() << "x: " << x << "\r\ny: " << y << endl;    
    MainWindow w;
    w.setMyMainWindow(&w);
    w.initNodes();
    w.setFixedSize(x,y);
    //QPushButton
  w.show();

//  bar[0] = new QProgressBar( &w);bar[1] = new QProgressBar( &w);bar[2] = new QProgressBar( &w);
//  bar[3] = new QProgressBar( &w);bar[4] = new QProgressBar( &w);bar[5] = new QProgressBar( &w);
//  bar[0]->setGeometry(x1 - 25,y_1 + 25, 118, 23);
//  bar[1]->setGeometry(x2 - 25,y2 + 25, 118, 23);
//  bar[2]->setGeometry(x3+50,y3 - 50, 118, 23);
//  bar[3]->setGeometry(x4-25,y4+ 25, 118, 23);
//  bar[4]->setGeometry(x5-25,y5+ 25, 118, 23);
//  bar[5]->setGeometry(x6-25,y6+25, 118, 23);
//  for(int i = 0; i < 6; i++){
//          bar[i]->setRange(0, 100);
//          bar[i]->setValue(98);
//          bar[i]->show();
//      }
//   ptable = new QTableWidget( &w);



    return a.exec();
}

