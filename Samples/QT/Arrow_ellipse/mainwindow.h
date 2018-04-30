#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include <QProgressBar>
#include <QTableWidget>
#include "node.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
     MainWindow * my_w;
    explicit MainWindow(QWidget *parent = 0);
    void createRouteTableAt(QTableWidget * myTable,QPoint *table_pos,QString *tablePacketStr);
    MainWindow * getMyMainWindow(void);
    void setMyMainWindow(MainWindow * w);
    ~MainWindow();
    void initNodes();
    node myNodes[6];
    QTimer *asset1Timer,*asset2Timer;
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QextSerialPort port;
    QMessageBox error;
    QVector<QPoint> tablePoints;
    QVector<QPoint> barPoints;
    QVector<QPoint> polyPoints;
    QVector<QPoint> drawPoints;
    QVector<QPoint> my_points;
    QVector<QPoint> my_fail_points;
    QStringList lk_list, list, fail_list;
    QPoint assetPoint1,assetPoint2;


private slots:
    void on_pushButton_close_clicked();
    void on_pushButton_open_clicked();
    void receive();
   // void paintEvent(QPaintEvent *e);
public slots:
    void asset1Timeout();
    void asset2Timeout();
    //node failtimer timeouts
    void node2Timeout(void);

protected:
    void paintEvent(QPaintEvent *e);
};
QSize myGetQTableWidgetSize(QTableWidget *t);
#endif // MAINWINDOW_H
