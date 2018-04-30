#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <QMainWindow>
#include <QtGui>
#include <QtCore>
namespace Ui {
class notepad;
}

class notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit notepad(QWidget *parent = 0);
    ~notepad();
    void paintEvent(QPaintEvent *);
    QTimer * timer;
    notepad * my_w;
    notepad * getMyMainWindow(void);
    void setMyMainWindow(notepad * w);
    void createRouteTableAt(QPoint *table_pos,QString *tablePacketStr);

private slots:
    void on_pushButton_clicked();

    void on_add_table_button_clicked();

    void on_pushButton_2_clicked();


private:
    Ui::notepad *ui;
};

static QSize myGetQTableWidgetSize(QTableWidget *t);
#endif // NOTEPAD_H
