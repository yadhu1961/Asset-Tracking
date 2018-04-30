#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineF>
#include <QPixmap>
#include <QDesktopWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
#include "main.h"


extern QProgressBar *bar[6];
 QPoint asset_p1, asset_p2, asset_p;
 extern QTableWidget *ptable;
extern int x1, x2, x3, x4, x5, x6;
extern int y_1, y2, y3, y4, y5, y6;
extern int x, y;
//uint16_t list[20];
const char *my_str;
int asset1 = 0;
int asset2 = 0;
int asset = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_NoSystemBackground);

    QPoint p1,p2,p3,p4,p5,p6;
    p1=QPoint(::x1,::y_1);
    p2=QPoint(::x2,::y2);
    p3=QPoint(::x3,::y3);
    p4=QPoint(::x4,::y4);
    p5=QPoint(::x5,::y5);
    p6=QPoint(::x6,::y6);
    polyPoints<<p1<<p2<<p3<<p4<<p5<<p6;
//    polyPoints << QPoint(::x1,::y_1);
//    polyPoints << QPoint(::x2,::y2);
//    //polyPoints << QPoint(::x3,::y3 - 50);
//    polyPoints << QPoint(::x3,::y6);
//    polyPoints << QPoint(::x4,::y4);
//    polyPoints << QPoint(::x5,::y5);
//    polyPoints << QPoint(::x6,::y6);

    tablePoints << QPoint(p1.x(),p1.y());        //QPoint(::x1,::y_1);
    tablePoints << QPoint(p2.x()+35,p2.y()+25) ;         //QPoint(::x2 + 35,::y2 + 25);
    tablePoints << QPoint(p3.x()+35,p3.y()+25) ;         //QPoint(::x4 + 35,::y6 + 25);
    tablePoints << QPoint(p4.x()-235,p4.y()+25) ;         //QPoint(::x4 - 235,::y4 + 25);
    tablePoints << QPoint(p5.x()+35,p5.y()+25) ;        //QPoint(::x5 + 35,::y5 + 25);
    tablePoints << QPoint(p6.x()-235,p6.y()+25) ;         //QPoint(::x6 - 235,::y6 + 25);
    barPoints <<p1<<p2<<p3<<p4<<p5<<p6;
    foreach(QPoint point, polyPoints)
        drawPoints << point;

    qDebug() << polyPoints;
    //lk_list << "1234" << "5678" << "0123" << "4567" << "8901" << "2345";
    lk_list << "903d" << "9073" << "566a" << "5415" << "9027" << "8ac3";


    //setAttribute(Qt::WA_PaintOnScreen);
   // ui->setAutoFillBackground(true);

    ui->setupUi(this);

    QPixmap pix(":/Images/legend.png");
    ui->label_pix->setPixmap(pix);

    QPalette pal = ui->textBrowser_2->palette();
    pal.setColor(QPalette::Base, QColor(0,0,0,0));
    ui->textBrowser_2->setPalette(pal);

    // Get all available COM Ports and store them in a QList.
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    // Read each element on the list, but
    // add only USB ports to the combo box.
   for (int i = 0; i < ports.size(); i++) {
       //if (ports.at(i).portName.contains("USB")){
#ifdef windowsOS
       if (ports.at(i).portName.contains("COM")){
#else
       if (ports.at(i).portName.contains("USB")){
#endif
            ui->comboBox_Interface->addItem(ports.at(i).portName.toLocal8Bit().constData());
        }
    }
    // Show a hint if no USB ports were found.
    if (ui->comboBox_Interface->count() == 0){
        ui->textEdit_Status->insertPlainText("No USB ports available.\nConnect a USB device and try again.");
    }
}
void MainWindow::initNodes()
{
    int i;
    for(i=0;i<6;i++)
    {
        myNodes[i].nodeAddr=lk_list.at(i);
        myNodes[i].nodePos=polyPoints.at(i);
        myNodes[i].tablePos=tablePoints.at(i);
        myNodes[i].pTable = new QTableWidget(this->getMyMainWindow());
        myNodes[i].pTable->hide();
        myNodes[i].pBar= new QProgressBar(this->getMyMainWindow());
        myNodes[i].pBar->setGeometry(myNodes[i].nodePos.x()-50,myNodes[i].nodePos.y()+25,90, 35);
//        myNodes[i].barPos=barPoints.at(i);
        myNodes[i].pBar->setRange(0,100);
        myNodes[i].pBar->setValue(98);
//        myNodes[i].failTimer=new QTimer(this->getMyMainWindow());
    }

    //timer creation
    // create a timer
    asset1Timer = new QTimer(this->getMyMainWindow());
    asset2Timer = new QTimer(this->getMyMainWindow());

    // setup signal and slot
    connect(asset1Timer, SIGNAL(timeout()),this, SLOT(asset1Timeout()));
    connect(asset2Timer, SIGNAL(timeout()),this, SLOT(asset2Timeout()));

    //connect failtimer signal to slot
    connect(myNodes[1].failTimer,SIGNAL(timeout()),this,SLOT(node2Timeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_open_clicked()
{
    port.setQueryMode(QextSerialPort::EventDriven);
    #ifdef windowsOS
    port.setPortName(ui->comboBox_Interface->currentText());
    #else
    port.setPortName("/dev/" + ui->comboBox_Interface->currentText());
    #endif
    qDebug() << ui->comboBox_Interface->currentText();
    port.setBaudRate(BAUD115200);
    port.setFlowControl(FLOW_OFF);
    port.setParity(PAR_NONE);
    port.setDataBits(DATA_8);
    port.setStopBits(STOP_1);
    port.open(QIODevice::ReadWrite);
  //  qDebug() << "Line 61";
    if (!port.isOpen())
    {
        error.setText("Unable to open port!");
        error.show();
        return;
    }
 //   qDebug() << "Line 68";
    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(receive()));


    ui->pushButton_close->setEnabled(true);
    ui->pushButton_open->setEnabled(false);
    ui->comboBox_Interface->setEnabled(false);

}

void MainWindow::on_pushButton_close_clicked()
{
    if (port.isOpen())port.close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    ui->comboBox_Interface->setEnabled(true);

    //stop timers
    asset2Timer->stop();
    asset1Timer->stop();

}

void MainWindow::receive()
{
    char ch;

//    char msg[50];
//    int counter = 0;
    static QString str;
    while (port.getChar(&ch))
    {
        //qDebug() << "Line 130";
        str.append(ch);
//        msg[counter++] = ch;
        if (ch == '\n')     // End of line, start decoding
        {
            //str.remove("\n", Qt::CaseSensitive);
            str.remove("\r", Qt::CaseSensitive);
            //ui->textEdit_Status->append(str);

//            qDebug() << "Str value: " << str;
            if (str.contains("4153534554"))         // Asset packet
            {               
                str.remove("4153534554", Qt::CaseInsensitive);
               // str.remove(",", Qt::CaseInsensitive);

                qDebug() << "Str value: " << str;
                const QByteArray data = str.toLocal8Bit();
                my_str = data.data();
                qDebug() << my_str;
                char c = 0;
                char arr[5];
                int j = 0;


                do
                {
                    c = my_str[j];
                    memcpy(arr,my_str + j,4);
                    arr[4] = '\0';
                    // qDebug() << arr;
                    //qDebug() << "Line 198";
                    j +=4;
                    list << arr;
                    // qDebug() << list;
                    //  k++;
                }while(c!= '\n');
                list.removeLast();
                int u = list.size();
                qDebug() << u;
                qDebug() << list;
                // qDebug() << list;
                //qDebug() << "Str value: " << str;
                if(!list.isEmpty()){
                    my_points.remove(0,my_points.size());
                    qDebug() << "Line 151" << endl << my_points;
                    for (int i=0; i < list.size(); i++){
                        if (lk_list.contains(list.at(i), Qt::CaseInsensitive))
                        {
                            //   qDebug() << lk_list.indexOf(list.at(i));
                            my_points << polyPoints.at(lk_list.indexOf(list.at(i)));
                            //
                            qDebug() << "Line 157" << my_points;
                        }
                    }
                    for (int i=0; i < (list.size()-1); i++)
                    {
                        if (lk_list.contains(list.at(i), Qt::CaseInsensitive))
                        {
                           int index=lk_list.indexOf(list.at(i));
                           myNodes[index].failTimer->start(20000);
                           myNodes[index].isFailed=0;
                        }
                    }


                    if(list.at(1) == "8ef5")
                    {
                        asset1 = 1;
                        asset=1;
                        asset1Timer->start(20000);
                        assetPoint1 =polyPoints.at(lk_list.indexOf(list.at(2)));;
                        ui->textEdit_Status->clear();
                        ui->textEdit_Status->insertPlainText("Asset ");
                        ui->textEdit_Status->insertPlainText(list.at(1));
                        ui->textEdit_Status->insertPlainText(" route received \r\n");
                    }
                    else if(list.at(1) == "9058")
                    {
                        asset2 = 1;
                        asset=2;
                        asset2Timer->start(20000);
                        assetPoint2= polyPoints.at(lk_list.indexOf(list.at(2)));;;//my_points.at(0);
                        ui->textEdit_Status->clear();
                        ui->textEdit_Status->insertPlainText("Asset ");
                        ui->textEdit_Status->insertPlainText(list.at(1));
                        ui->textEdit_Status->insertPlainText(" route received \r\n");
                    }
                    while(!list.empty()) list.removeLast();

                    //qDebug() << list;


                }
            }
            else if (str.contains("5441424c45"))    //Table packet
            {
                str.remove("5441424c45", Qt::CaseInsensitive);
                str.remove("\n", Qt::CaseInsensitive);

                qDebug() << "Str value: " << str;
                QString availStr =str;
                QStringList avail_list;
                //remove node addr and store if not available in your availlist
                //then remove neighbor addr and store
                //dicard hop count
                int size=availStr.size();
                if((size-4)%8==0)
                {

                    avail_list<<availStr.left(4);
                    availStr.remove(0,4);
                    int rows=(size-4)/8;
                    for(int i=0;i<rows;i++)
                    {
                        avail_list<<(availStr.left(4));
                        availStr.remove(0,8);
                    }
                }
                else
                {
                    qDebug()<<"pkt currrupted";
                }

                if(!avail_list.isEmpty()){
                    my_points.remove(0,my_points.size());
                    qDebug() << "Line 151" << endl << my_points;
                    for (int i=0; i < avail_list.size(); i++){
                        if (lk_list.contains(avail_list.at(i), Qt::CaseInsensitive))
                        {
                            int index=lk_list.indexOf(avail_list.at(i));
                           myNodes[index].failTimer->start(20000);
                           myNodes[index].isFailed=0;
//                         if( my_fail_points.contains(myNodes[index].nodePos))
//                         {
//                             int id = my_fail_points.indexOf(myNodes[index].nodePos);
//                             my_fail_points.remove(id, 1);
//                         }
                        }
                    }
                }


                QPoint *pTablePos;
//                pTablePos = new QPoint(100,100);
                QString *tablepacketstr;
               // QString linestr = "nodealph1234beta3456gama7890"; //this is only for testing
                 QString linestr = str;
                tablepacketstr=&linestr;
                QTableWidget* ptable;
//                ptable = new QTableWidget(this->getMyMainWindow());
                ui->textEdit_Status->clear();
                ui->textEdit_Status->insertPlainText("TABLE PACKET\r\n ");
                ui->textEdit_Status->insertPlainText(tablepacketstr->left(4));

               QString temp_nodeAddr = str.left(4);

                for(int g = 0; g < 6; g++)
                {
                    if(temp_nodeAddr == myNodes[g].nodeAddr)
                    {
                        pTablePos=&myNodes[g].tablePos;
                        ptable=myNodes[g].pTable;
                    }
                }
                createRouteTableAt(ptable,pTablePos,tablepacketstr);

            }



            else if(str.contains("Fail"))
            {

                str.remove("Fail,", Qt::CaseSensitive);
                const QByteArray data = str.toLocal8Bit();
                my_str = data.data();
                //qDebug() << my_str << "Line 249";
                char c = 0;
                char arr[5];
                int j = 0;
               // int k = 0;
                do
                {
                    c = my_str[j];
                    memcpy(arr,my_str + j,4);
                    arr[4] = '\0';

                 //   qDebug() << arr;
                    j +=4;
                    list << arr;
                   // qDebug() << list;
                   // qDebug() << list;
                  //  k++;
                }while(c!= '\n');
                list.removeLast();

                if(!list.isEmpty()){

                        if (lk_list.contains(list.at(0), Qt::CaseInsensitive))
                        {                            
                            my_fail_points << polyPoints.at(lk_list.indexOf(list.at(0)));
                            fail_list << lk_list.at(lk_list.indexOf(list.at(0)));
//                            bar[lk_list.indexOf(list.at(0))]->setValue(0);

                            qDebug() << my_fail_points;
                        }
                    }
                ui->textEdit_Status->clear();
                ui->textEdit_Status->insertPlainText("Node ");
                ui->textEdit_Status->insertPlainText(list.at(0));
                ui->textEdit_Status->insertPlainText(" Failed \r\n");
                while(!list.empty()) list.removeLast();
            }
            else if(str.contains("42415454455259")) // Battery message
            {

                str.remove("42415454455259", Qt::CaseSensitive);
                const QByteArray data = str.toLocal8Bit();
                my_str = data.data();
                //qDebug() << my_str << "Line 249";
                char c = 0;
                char arr[5];
                int j = 0;
               // int k = 0;
                do
                {
                    c = my_str[j];
                    memcpy(arr,my_str + j,4);
                    arr[4] = '\0';
                    j +=4;
                    list << arr;
                }while(c!= '\n');
                list.removeLast();
                qDebug()<<list;
                if(!list.isEmpty()){

                        if (lk_list.contains(list.at(0), Qt::CaseInsensitive))
                        {
                            if(list.at(1) == "b004")    //Button press for 1.2V
                            {
                                qDebug() << "Line 275";
//                            bar[lk_list.indexOf(list.at(0))]->setValue(50);
                                myNodes[lk_list.indexOf(list.at(0))].pBar->setValue(50);

                            }
                            else
                            {
//                            bar[lk_list.indexOf(list.at(0))]->setValue(98);
                                myNodes[lk_list.indexOf(list.at(0))].pBar->setValue(98);
                            }
                        }
                    }
                ui->textEdit_Status->clear();
                ui->textEdit_Status->insertPlainText("Node ");
                ui->textEdit_Status->insertPlainText(list.at(0));
                ui->textEdit_Status->insertPlainText(" Battery level \r\n");
                while(!list.empty()) list.removeLast();
            }
            this->update();
           // this->repaint();
            str.clear();
            }
            //this->repaint();    // Update content of window immediately

        }

  //  this->repaint();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    int radius = 25;
  //  QRegExp rx("(\\d+)");
    painter.eraseRect(0,0,::x,::y);
    QString myString;
    //QStringList list,;

    // make a pen
    QPen my_pen_red; QPen my_pen_green; QPen my_pen_black; QPen my_pen_yellow; QPen my_pen_magneta;

    QPen my_pen_blue;
    my_pen_magneta.setWidth(4);
    my_pen_red.setWidth(4);
    my_pen_yellow.setWidth(4);
    my_pen_black.setWidth(4);
    my_pen_green.setWidth(4);
    my_pen_blue.setWidth(4);
    my_pen_magneta.setColor(Qt::darkMagenta);
    my_pen_red.setColor(Qt::red);
    my_pen_yellow.setColor(Qt::yellow);
    my_pen_black.setColor(Qt::black);
    my_pen_green.setColor(Qt::green);
    my_pen_blue.setColor(Qt::blue);

    QFont font( "Helvetica" );
    font.setPointSize( 16 );

    painter.setPen(my_pen_black);
   // painter.drawLine(polyPoints.at(0), polyPoints.at(1));
    foreach(QPoint point, drawPoints)
    {
        painter.drawEllipse( point, radius, radius );
        painter.setFont( font );
        int new_x;
        int new_y;
        new_x = point.x()-25;
        new_y = point.y()-28;
        //qDebug() << lk_list.at(drawPoints.indexOf(point));
        QString s = lk_list.at(drawPoints.indexOf(point));
        point.setX(new_x);
        point.setY(new_y);
        painter.drawText(point, s);
    }
    painter.setPen(my_pen_red);
    //qDebug() << "Failpoints" << my_fail_points;


//    foreach(QPoint point, my_fail_points)
//    {
//        painter.drawEllipse( point, radius, radius );
////        painter.setFont( font );
////        int new_x;
////        new_x = point.x() + 26;
////        //qDebug() << lk_list.at(drawPoints.indexOf(point));
////        QString s = fail_list.at(my_fail_points.indexOf(point));
////        point.setX(new_x);
////        painter.drawText(point, s);
//    }
    for(int i=0;i<6;i++)
    {
        if(myNodes[i].isFailed==1)
            painter.drawEllipse( myNodes[i].nodePos, radius, radius );
    }

    painter.setPen(my_pen_green);
    painter.drawEllipse( polyPoints.at(0), radius, radius );


// ASSET painting
//    if(!my_points.empty())
    if(1)
    {
         font.setPointSize(14);
         painter.setFont( font );
        int asset_x;
        if(asset1 == 1)
        {
            asset_p = assetPoint1;
            asset_x = assetPoint1.x() - 50;
            asset_p.setX(asset_x);
            asset_p1 = asset_p;
            painter.setBrush(Qt::blue);
            painter.setPen(my_pen_blue);
            painter.drawEllipse(asset_p1, 10, 10);
            asset_p1.setY(asset_p1.y()-12);
            asset_p1.setX(asset_p1.x()-15);
            painter.drawText(asset_p1, "8ef5");

        }
        if(asset2 == 1)
        {
            asset_p = assetPoint2;
            asset_x = assetPoint2.x() + 50;
            asset_p.setX(asset_x);
            asset_p2 = asset_p;
            painter.setPen(my_pen_magneta);
            painter.setBrush(Qt::darkMagenta);
            painter.drawEllipse(asset_p2, 10, 10);
            asset_p2.setY(asset_p2.y()-12);
            asset_p2.setX(asset_p2.x()-10);
            painter.drawText(asset_p2, "9058");
        }

        font.setPointSize(16);
        painter.setFont( font );
    }


    if(asset == 1)
        painter.setPen(my_pen_blue);
    else if(asset == 2)
        painter.setPen(my_pen_magneta);


    for(int k = 0; k < my_points.size() - 1; k++)
    {        
            int new_x1, new_x2;
            QPoint new_p1, new_p2;
            new_p1 = my_points.at(k);
            new_p2 = my_points.at(k+1);
            if(asset == 2){
            new_x1 = new_p1.x() + 3; new_x2 = new_p2.x() + 3;
            new_p1.setX(new_x1);new_p2.setX(new_x2);
            }

            //qDebug() << lk_list.at(drawPoints.indexOf(point));
            painter.drawLine(new_p1, new_p2);
       // qDebug() << my_points;
    }

}
/*
gets table size once it is created
*/
QSize myGetQTableWidgetSize(QTableWidget *t) {
   int w = t->verticalHeader()->width() + 4; // +4 seems to be needed
   for (int i = 0; i < t->columnCount(); i++)
      w += t->columnWidth(i); // seems to include gridline (on my machine)
   int h = t->horizontalHeader()->height() + 4;
   for (int i = 0; i < t->rowCount(); i++)
      h += t->rowHeight(i);
   return QSize(w, h);
}
void MainWindow::createRouteTableAt(QTableWidget * myTable,QPoint *pTablePos,QString *pTablePacketStr)
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
//    table=  new QTableWidget(this->getMyMainWindow());
    table=  myTable;
    qDebug()<<pTablePacketStr->size()<<(*pTablePacketStr);
    listsize=linestr.size();
    rows=(listsize-4)/8;
    qDebug()<<rows;
//    if(rows%8==0)
//        qDebug()<<"error, packet may be corruped, packet is not complete";
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
        int b0,b1;
        b0=n.left(2).toInt(0,16);
        n.remove(0,2);
        b1=n.left(2).toInt(0,16);
        n.remove(0,2);
        qDebug()<<b0<<b1;
        n=QString::number(b1);
        n.append(QString::number(b0));
        QTableWidgetItem *hopcount = new QTableWidgetItem(n);
        table->setItem(i, 1, hopcount);
        linestr.remove(0,4);
    }
    table->setMaximumSize(myGetQTableWidgetSize(table));
    table->setMinimumSize(table->maximumSize()); // optional
    table->show();
}

MainWindow* MainWindow::getMyMainWindow(void)
{
    return this->my_w;
}
void MainWindow::setMyMainWindow(MainWindow * w)
{
   this->my_w=w;
}
void MainWindow::asset1Timeout(void)
{
    asset1 = 0;
    qDebug()<<"asset 1 timeout";
    QProcess process;
    process.start("play /home/ga83jeb/group3/astra/QT/Arrow_ellipse/beep-01a.wav");
    //error.setText("ASSET 1 LOST!");
    process.waitForFinished();
    process.close();
    ui->textEdit_Status->clear();
    ui->textEdit_Status->insertPlainText("Asset 1 8EF5 lost!");
    asset1Timer->stop();
    //error.show();
}
void MainWindow::asset2Timeout(void)
{
    asset2 = 0;
    qDebug()<<"asset 2 timeout";
    QProcess process;
    process.start("play /home/ga83jeb/group3/astra/QT/Arrow_ellipse/beep-01a.wav");
    process.waitForFinished();
    process.close();
    //error.setText("ASSET 2 LOST!");
    ui->textEdit_Status->insertPlainText("Asset 2 9058 lost!");
    asset2Timer->stop();
    //error.show();
}
void MainWindow::node2Timeout(void)
{
//    fail_list<<lk_list.at(1);
//    my_fail_points<<myNodes[1].nodePos;
//    myNodes[1].failTimer->stop();
//    error.setText("9073 failed");
//    error.show();
}
