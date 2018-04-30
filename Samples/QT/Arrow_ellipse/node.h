#ifndef NODE_H
#define NODE_H
#include <QtGui>
#include <QtCore>
#include <QProgressBar>
#include <QTableWidget>
#include <QTimer>

class node: public QObject
{
    Q_OBJECT
public:
    node();
public:
    QString nodeAddr;
    QPoint nodePos;
    QPoint tablePos;
    QPoint barPos;
    QProgressBar *pBar;
    QTableWidget *pTable;
    QTimer *failTimer;
    bool isFailed;
public slots:
    void myTimeout(void);
};
#endif // NODE_H
