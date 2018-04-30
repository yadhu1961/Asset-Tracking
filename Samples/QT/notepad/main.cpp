#include "notepad.h"
#include "mytimer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    notepad w;
    w.setMyMainWindow(&w);
    w.show();

    return a.exec();
}
