#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineF>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>

extern int x,y;
u_int16_t list[20];
const char *kirchki_str;
int asset = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_NoSystemBackground);


    polyPoints << QPointF(::x/2,50);
    polyPoints << QPointF(::x/4*3,::y/4);
    polyPoints << QPointF(::x/2,::y -50);
    polyPoints << QPointF(::x/4,::y/4);
    polyPoints << QPointF(::x/4*3,::y/4*3);
    polyPoints << QPointF(::x/4,::y/4 * 3);

    foreach(QPointF point, polyPoints)
        drawPoints << point;

    qDebug() << polyPoints;
    lk_list << "1234" << "5678" << "0123" << "4567" << "8901" << "2345";

    //setAttribute(Qt::WA_PaintOnScreen);
   // ui->setAutoFillBackground(true);

    ui->setupUi(this);
    // Get all available COM Ports and store them in a QList.
//    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

//    // Read each element on the list, but
//    // add only USB ports to the combo box.
//    for (int i = 0; i < ports.size(); i++) {
//        if (ports.at(i).portName.contains("USB")){
//            ui->comboBox_Interface->addItem(ports.at(i).portName.toLocal8Bit().constData());
//        }
//    }
    // Show a hint if no USB ports were found.
//    if (ui->comboBox_Interface->count() == 0){
//        ui->textEdit_Status->insertPlainText("No USB ports available.\nConnect a USB device and try again.");
//    }
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
    QString portname = "/dev/";
    portname.append("ttyUSB1");
    port.setPortName(portname);
    port.setBaudRate(BAUD115200);
    port.setFlowControl(FLOW_OFF);
    port.setParity(PAR_NONE);
    port.setDataBits(DATA_8);
    port.setStopBits(STOP_1);
    port.open(QIODevice::ReadWrite);
  //  qDebug() << "Line 61";
    if (!port.isOpen())
    {
        qDebug()<<("Unable to open port!");
        //error.show();
        return;
    }
 //   qDebug() << "Line 68";
    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(receive()));


    ui->pushButton_close->setEnabled(true);
    ui->pushButton_open->setEnabled(false);
    //ui->comboBox_Interface->setEnabled(false);
}

void MainWindow::on_pushButton_close_clicked()
{
    if (port.isOpen())port.close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    //ui->comboBox_Interface->setEnabled(true);

}

void MainWindow::receive()
{
    char ch;

    char msg[50];
    int counter = 0;
    static QString str;
    while (port.getChar(&ch))
    {
        str.append(ch);
        msg[counter++] = ch;
        if (ch == '\n')     // End of line, start decoding
        {

            //str.remove("\n", Qt::CaseSensitive);
            str.remove("\r", Qt::CaseSensitive);
            //ui->textEdit_Status->append(str);

            //qDebug() << "Str value: " << str;
            if (str.contains("Asset"))
            {
                str.remove("Asset,", Qt::CaseSensitive);
                str.remove(",", Qt::CaseSensitive);


                const QByteArray data = str.toLocal8Bit();
                kirchki_str = data.data();
                qDebug() << kirchki_str;
                char c = 0;
                char asset_addr[5];
                int j = 0;

                do
                {
                    c = kirchki_str[j];
                    memcpy(asset_addr,kirchki_str + j,4);
                    asset_addr[4] = '\0';
                   // qDebug() << asset_addr;
                    //qDebug() << "Line 198";
                    j +=4;
                    list << asset_addr;
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

                    /*foreach(QPointF point, my_points)
                        my_points.removeAll(point);*/
                    my_points.remove(0,my_points.size());
                     qDebug() << "Line 151" << endl << my_points;
                    for (int i=0; i < list.size(); i++){
                        if (lk_list.contains(list.at(i), Qt::CaseInsensitive))
                        {
                         //   qDebug() << lk_list.indexOf(list.at(i));
                            my_points << polyPoints.at(lk_list.indexOf(list.at(i)));
                            qDebug() << "Line 157" << my_points;
                        }
                    }
                    if(list.at(0) == "0001")
                        asset = 1;
                    else if(list.at(0) == "0002")
                        asset = 2;
                    while(!list.empty()) list.removeLast();

                    //qDebug() << list;


                }
            }

            else if(str.contains("Fail"))
            {

                str.remove("Fail,", Qt::CaseSensitive);
                const QByteArray data = str.toLocal8Bit();
                kirchki_str = data.data();
                //qDebug() << kirchki_str << "Line 249";
                char c = 0;
                char asset_addr[5];
                int j = 0;
               // int k = 0;
                do
                {
                    c = kirchki_str[j];
                    memcpy(asset_addr,kirchki_str + j,4);
                    asset_addr[5] = '\0';

                 //   qDebug() << asset_addr;
                    j +=4;
                    list << asset_addr;
                   // qDebug() << list;
                   // qDebug() << list;
                  //  k++;
                }while(c!= '\n');
                list.removeLast();

                if(!list.isEmpty()){

                        if (lk_list.contains(list.at(0), Qt::CaseInsensitive))
                        {
                            qDebug() << "Line 270";
                         //   qDebug() << lk_list.indexOf(list.at(i));
                            my_fail_points << polyPoints.at(lk_list.indexOf(list.at(0)));
                            fail_list << lk_list.at(lk_list.indexOf(list.at(0)));
                           // drawPoints.remove(lk_list.indexOf(list.at(0)));
                            //lk_list.removeAt();
                           // qDebug() << drawPoints;
                            qDebug() << my_fail_points;
                           // painter.setPen(my_pen_red);
                           // painter.drawEllipse( my_fail_points.at(0), radius, radius );

                         //   qDebug() << polyPoints.at(lk_list.indexOf(list.at(i)));
                        }
                    }
                while(!list.empty()) list.removeLast();
            }
            this->update();
           // this->repaint();
            str.clear();

//            int u = list.size();
//            qDebug() << u;


//            for(int k = 0; k < lk_list.size(); k++)
//                lk_list.removeAt(k);


            }
            //this->repaint();    // Update content of window immediately

        }
  //  this->repaint();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    int radius = 25;
    //qDebug() << "Line 222";
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
    foreach(QPointF point, drawPoints)
    {
        //qDebug() << "Line 137" << lk_list.at(drawPoints.indexOf(point));

        painter.drawEllipse( point, radius, radius );
        painter.setFont( font );
        qreal new_x;
        new_x = point.x() + 26;
        //qDebug() << lk_list.at(drawPoints.indexOf(point));
        QString s = lk_list.at(drawPoints.indexOf(point));
        point.setX(new_x);
        painter.drawText(point, s);
       // qDebug() << "Line 150" << lk_list.at(drawPoints.indexOf(point));
       //

        //
    }
    painter.setPen(my_pen_red);
    //qDebug() << "Failpoints" << my_fail_points;
    foreach(QPointF point, my_fail_points)
    {
        painter.drawEllipse( point, radius, radius );
        painter.setFont( font );
        qreal new_x;
        new_x = point.x() + 26;
        //qDebug() << lk_list.at(drawPoints.indexOf(point));
        QString s = fail_list.at(my_fail_points.indexOf(point));
        point.setX(new_x);
        painter.drawText(point, s);
    }

    painter.setPen(my_pen_green);
    painter.drawEllipse( polyPoints.at(0), radius, radius );

    if(asset == 1)
        painter.setPen(my_pen_blue);
    else if(asset == 2)
        painter.setPen(my_pen_magneta);
    for(int k = 0; k < my_points.size() - 1; k++)
    {        

            qreal new_x1, new_x2;
            QPointF new_p1, new_p2;
            new_p1 = my_points.at(k);
            new_p2 = my_points.at(k+1);
            if(asset == 2){
            new_x1 = new_p1.x() + 3; new_x2 = new_p2.x() + 3;
            new_p1.setX(new_x1);new_p2.setX(new_x2);}
            //qDebug() << lk_list.at(drawPoints.indexOf(point));
            painter.drawLine(new_p1, new_p2);
       // qDebug() << my_points;
    }
   // my_points.clear();


   // str.sprintf("%s", "Fail,8901");
    //list = str.split(QRegExp("\\W+"));
}
