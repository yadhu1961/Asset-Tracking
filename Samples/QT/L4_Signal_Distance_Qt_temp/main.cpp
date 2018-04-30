#include "mainwindow.h"
#include <QApplication>
#include<QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QProcess process;
    process.start("play /home/ga83jeb/group3/astra/QT/Arrow_ellipse/beep-01a.wav");

    MainWindow w;
    w.show();

    return a.exec();
}
