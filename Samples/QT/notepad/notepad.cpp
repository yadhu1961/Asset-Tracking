#include "notepad.h"
#include "ui_notepad.h"
#include <QApplication>
#include <qdebug.h>
#include <QString>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QProgressBar>
#include <QLineF>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};
notepad::notepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::notepad)
{

    timer = new QTimer(this);

    // setup signal and slot
//    connect(timer, SIGNAL(timeout()),
//          this, SLOT(update()));
    timer->start(1000);
    ui->setupUi(this);


}

notepad::~notepad()
{
    delete ui;
}

void notepad::on_pushButton_clicked()
{
     int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row+1);
    ui->tableWidget->scrollToBottom();
    QTableWidgetItem *index = new QTableWidgetItem(QString::number(0));
    ui->tableWidget->setItem(row-1, 0, index);
}
void notepad::paintEvent(QPaintEvent *)
{
    int x = 0;
    int y = 0;
    int radius=15;
//    static int counter;
    QDesktopWidget dw;
    QPainter painter(this);
    QPen pen;
    pen.setWidth(4);
    pen.setColor(Qt::darkMagenta);
    painter.setPen(pen);
    x=dw.width()*0.5;
    y=dw.height()*0.5;
//    qDebug() << "x: " << x << "\r\ny: " << y << endl;
    QVector<QPointF> polyPoints;
     polyPoints << QPointF(x/2,y/2);
     polyPoints << QPointF(x/4,y/4);
     polyPoints << QPointF(x/4,y/2);
     polyPoints << QPointF(x/2,y/4);
      for(int k = 0; k < polyPoints.size() - 1; k++)
     {
        qreal new_x1, new_x2;
        QPointF new_p1, new_p2;
        new_p1 = polyPoints.at(k);
        new_p2 = polyPoints.at(k+1);
        new_x1 = new_p1.x() + 3; new_x2 = new_p2.x() + 3;
        new_p1.setX(new_x1);new_p2.setX(new_x2);
        //qDebug() << lk_list.at(drawPoints.indexOf(point));
        painter.drawLine(new_p1, new_p2);
//        qDebug("paintEvent, %d", counter++);
     }

      foreach(QPointF point, polyPoints)
     {
        painter.setBrush(Qt::green);
       painter.drawEllipse( point, radius, radius );
//            SleeperThread::msleep(120) ;
       painter.setBrush(Qt::red);
      painter.drawEllipse( point, radius, radius );
//           SleeperThread::msleep(120) ;
//       qDebug() << "poly point " << point << endl;

   }
}

void notepad::on_add_table_button_clicked()
{
    QTableWidget * table;
    const QRect *rect;
    rect =new QRect(100,100,100,100);
    table=  new QTableWidget(this->getMyMainWindow());
    table->setGeometry(*rect);
    table->show();
    qDebug()<<table->minimumWidth();

}

notepad* notepad::getMyMainWindow(void)
{
    return this->my_w;
}
void notepad::setMyMainWindow(notepad * w)
{
   this->my_w=w;
}

void notepad::on_pushButton_2_clicked()
{
    QDebug deb = qDebug();
    QFile file("/home/ga83jok/Lab_Exercises/astra/QT/notepad/packet.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
    }

   QStringList wordList;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        wordList.append(line);
    }
    QPoint *tablePos;
    tablePos = new QPoint(100,100);
    QString *tablepacketstr;
    QString linestr = "nodealph1234beta3456gama7890";
    tablepacketstr=&linestr;
    createRouteTableAt(tablePos,tablepacketstr);

    /*


    for(i=0;i<wordList.size();i++)
    {

        if(wordList.at(i).contains("TABLE"))
        {*/
//            int j;

            /*
            linestr=wordList.at(i);
            qDebug()<<linestr.size()<<linestr;

            linestr.remove("TABLE");
            qDebug()<<linestr.size()<<linestr;
            linestr.remove(",");
            linestr.remove("\"");
            qDebug()<<linestr.size()<<linestr;
            listsize=linestr.size();

//            temp=linestr.left(4);
             qDebug()<<linestr.size()<<linestr;
            //remove TABLE*/
//        }
//    }
}


/*
gets table size once it is created
*/
static QSize myGetQTableWidgetSize(QTableWidget *t) {
   int w = t->verticalHeader()->width() + 4; // +4 seems to be needed
   for (int i = 0; i < t->columnCount(); i++)
      w += t->columnWidth(i); // seems to include gridline (on my machine)
   int h = t->horizontalHeader()->height() + 4;
   for (int i = 0; i < t->rowCount(); i++)
      h += t->rowHeight(i);
   return QSize(w, h);
}

void notepad::createRouteTableAt(QPoint *pTablePos,QString *pTablePacketStr)
{
    int i;
    int listsize;
//    QString linestr = "nodealph1234beta3456gama7890";
    QString linestr;
    QTableWidget * table;
    const QRect *rect;
    const QSize * pTableSize;
    int rows;
    linestr=QString(*pTablePacketStr);
    pTableSize=new QSize(200,200);
    rect =new QRect(*pTablePos,*pTableSize);
    table=  new QTableWidget(this->getMyMainWindow());
    qDebug()<<pTablePacketStr->size()<<(*pTablePacketStr);
    listsize=linestr.size();
    rows=(listsize-4)/8;
    qDebug()<<rows;
    if(rows%8==0)
        qDebug()<<"error, packet may be corruped, packet is not complete";
    table->setRowCount(rows);
    table->setColumnCount(2);
    QTableWidgetItem *header1 = new QTableWidgetItem("Neighbour");
    table->setHorizontalHeaderItem(0,header1);
    QTableWidgetItem *header2 = new QTableWidgetItem("HopCount");
    table->setHorizontalHeaderItem(1,header2);
    table->setColumnWidth(0,80);

    table->setGeometry(*rect);
    table->scrollToBottom();
    linestr.remove(0,4);//remove node string
    for(i=0;i<rows;i++)
    {
        QString n;
        n=linestr.left(4);
        qDebug()<<n;
        QTableWidgetItem *neighbour = new QTableWidgetItem(n);
        table->setItem(i, 0, neighbour);
        linestr.remove(0,4);
        n=linestr.left(4);
        qDebug()<<n;
        QTableWidgetItem *hopcount = new QTableWidgetItem(n);
        table->setItem(i, 1, hopcount);
        linestr.remove(0,4);
    }
    table->setMaximumSize(myGetQTableWidgetSize(table));
    table->setMinimumSize(table->maximumSize()); // optional
    table->show();
}
