#include "mainwindow.h"
#include <QApplication>
#include <qdebug.h>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QProgressBar>
//#include <QtWidgets>
int x = 0;
int y = 0;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDesktopWidget dw;
    x=dw.width()*0.7;
    y=dw.height()*0.8;



//    QProgressBar bar(0);
//    bar.setRange(0, 100);
//    bar.setValue(0);
//    bar.setGeometry(0,0,10,10);
//    bar.show();

    qDebug() << "x: " << x << "\r\ny: " << y << endl;
    qDebug() << "w: " << dw.width() << "\r\nh: " << dw.height() << endl;
    MainWindow w;
    w.setFixedSize(x,y);
    //QPushButton
  w.show();
  QProgressBar *bar[6];
  bar[0] = new QProgressBar( &w);bar[1] = new QProgressBar( &w);bar[2] = new QProgressBar( &w);
  bar[3] = new QProgressBar( &w);bar[4] = new QProgressBar( &w);bar[5] = new QProgressBar( &w);
  bar[0]->setGeometry(x/2 - 25,75, 118, 23);
  bar[1]->setGeometry((x/4*3) - 25,(y/4) + 25, 118, 23);
  bar[2]->setGeometry((x/2)+50,y - 50, 118, 23);
  bar[3]->setGeometry((x/4)-25,(y/4)+ 25, 118, 23);
  bar[4]->setGeometry((x/4*3)-25,(y/4*3)+ 25, 118, 23);
  bar[5]->setGeometry((x/4)-25,(y/4 * 3)+25, 118, 23);
  for(int i = 0; i < 6; i++){
          bar[i]->setRange(0, 100);
          bar[i]->setValue(98);
          bar[i]->show();
      }



    return a.exec();
}

