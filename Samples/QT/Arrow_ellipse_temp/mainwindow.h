#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include "qextserialport.h"
#include "qextserialenumerator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QextSerialPort port;
    QVector<QPointF> polyPoints;
    QVector<QPointF> drawPoints;
    QVector<QPointF> my_points;
    QVector<QPointF> my_fail_points;
    QStringList lk_list, list, fail_list;


private slots:
    void on_pushButton_close_clicked();
    void on_pushButton_open_clicked();
    void receive();
   // void paintEvent(QPaintEvent *e);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // MAINWINDOW_H
