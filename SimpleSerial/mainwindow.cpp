#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    usbDevice = new QSerialPort(this);
    connect(usbDevice,SIGNAL(readyRead()),this,SLOT(onSerialDataAvailable()));

    endOfMessage = "\n";
    baudrate = QSerialPort::Baud115200;

    serialDeviceIsConnected = false;
    getAvalilableSerialDevices();
    messageCounter = 0;



    dataSeries = new QLineSeries();

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(dataSeries);
    chart->createDefaultAxes();

    // Settings for the Plotter
    chart->setTitle("X-Y Plotter");
    maxChartPoints = 100;
    chartAxisRange = QRectF(0,0,1023,1023); // lowX, lowY, highX, highY
    setChartRange(chartAxisRange);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->setParent(this);
    QPoint chartPos(10,50);
    chartView->setGeometry(chartPos.x(),
                           chartPos.y(),
                           this->geometry().width()-2*chartPos.x(),
                           this->geometry().height()-chartPos.y());
    chartView->show();

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timerFinished()));

    averageTimeSpan = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete usbDevice;
    delete chart;
    delete dataSeries;
}
void MainWindow::getAvalilableSerialDevices()
{
    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
    serialComPortList.clear();
    ui->serialPortSelect_comboBox->clear();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        QString dbgStr = "Vendor ID: ";


	   if(serialPortInfo.hasVendorIdentifier())
	   {
		  dbgStr+= serialPortInfo.vendorIdentifier();
	   }
	   else
	   {
		  dbgStr+= " - ";
	   }
	   dbgStr+= "  Product ID: ";
	   if(serialPortInfo.hasProductIdentifier())
	   {
		  dbgStr+= serialPortInfo.hasProductIdentifier();
	   }
	   else
	   {
		  dbgStr+= " - ";
	   }
	   dbgStr+= " Name: " + serialPortInfo.portName();
	   dbgStr+= " Description: "+serialPortInfo.description();
	  qDebug()<<dbgStr;
	  serialComPortList.push_back(serialPortInfo);
	  ui->serialPortSelect_comboBox->addItem(serialPortInfo.portName() +" "+serialPortInfo.description());
    }
}
void MainWindow::serialWrite(QString message)
{
    if(serialDeviceIsConnected == true)
    {
        usbDevice->write(message.toUtf8()); // Send the message to the device
        qDebug() << "Message to device: "<<message;
    }
}
void MainWindow::serialRead()
{
    if(serialDeviceIsConnected == true)
    {
        serialBuffer.reserve(serialBuffer.size()*2);
        serialBuffer += usbDevice->readAll().toStdString(); // Read the available data
    }
}
void MainWindow::onSerialDataAvailable()
{
    if(serialDeviceIsConnected == true)
    {
        serialRead(); // Read a chunk of the Message
        //To solve that problem I send a end char "]" in My case. That helped my to know when a message is complete

        t1 = std::chrono::high_resolution_clock::now();
        while(serialBuffer.find(endOfMessage) != std::string::npos)
        {
            std::string message = serialBuffer.substr(0,serialBuffer.find(endOfMessage));
            //if(serialBuffer.indexOf("\n") != -1) //Message complete
            {
                //qDebug() << "Message from device: "<<message;
                //serialWrite("echoFromGui");

                //Do something with de message here
                QPointF point;
                try {
                    point.setX(std::stof(message.substr(0,message.find(" ")).c_str()));
                    point.setY(std::stof(message.substr(message.find(" ")+1,message.find_last_of(" ")-message.find(" ")).c_str()));
                    addChartPoint(point);
                } catch (...) {
                    qDebug() << "err";
                }
                messageCounter++;
            }
            serialBuffer = serialBuffer.substr(serialBuffer.find(endOfMessage)+endOfMessage.size());
        }
        t2 = std::chrono::high_resolution_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        averageTimeSpan = averageTimeSpan*0.9 + 0.1*time_span.count();
    }
}


void MainWindow::on_connect_button_clicked()
{
    if(serialDeviceIsConnected == false)
    {
        usbDevice->setPortName(serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].portName());
        deviceDescription = serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].description();
        qDebug() << "connecting to: "<<usbDevice->portName();
        if(usbDevice->open(QIODevice::ReadWrite))
        {
            //Now the serial port is open try to set configuration
            if(!usbDevice->setBaudRate(baudrate))        //Depends on your boud-rate on the Device
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setDataBits(QSerialPort::Data8))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setParity(QSerialPort::NoParity))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setStopBits(QSerialPort::OneStop))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setFlowControl(QSerialPort::NoFlowControl))
                qDebug()<<usbDevice->errorString();

            //If any error was returned the serial il corrctly configured

            qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " connected";
            serialDeviceIsConnected = true;
            timer->start(1000);
        }
        else
        {
            qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " not connected";
            qDebug() <<"         Error: "<<usbDevice->errorString();
        }
    }
    else
    {
        qDebug() << "Can't connect, another device is connected";
    }
}

void MainWindow::on_disconnect_button_clicked()
{
    if(serialDeviceIsConnected)
    {
        usbDevice->close();
        serialDeviceIsConnected = false;
        timer->stop();
        qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " closed";
    }
    else
    {
       qDebug() << "Can't disconnect, no device is connected";
    }
}
void MainWindow::on_timerFinished()
{
    qDebug() << averageTimeSpan*1000 << "ms / message \t"<< messageCounter << " messages / s";
    messageCounter = 0;
}
void MainWindow::addChartPoint(const QPointF &point)
{

    //chart->createDefaultAxes();
    //chartView->update();
    //chart->createDefaultAxes();
#ifdef DYNAMIC_AXIS_SCALE
    if(point.x() < chartAxisRange.x() ||
       point.x() > chartAxisRange.width() ||
       point.y() < chartAxisRange.y() ||
       point.y() > chartAxisRange.height())
    {
        if(point.x() < chartAxisRange.x())
            chartAxisRange.setX(point.x());
        else if(point.x() > chartAxisRange.width())
            chartAxisRange.setWidth(point.x());
        if(point.y() < chartAxisRange.y())
            chartAxisRange.setY(point.y());
        else if(point.y() > chartAxisRange.height())
            chartAxisRange.setHeight(point.y());

        setChartRange(chartAxisRange);
    }
#endif


    if(dataSeries->count() > maxChartPoints)
        dataSeries->remove(0);
    dataSeries->append(point);
}

void MainWindow::setChartRange(const QRectF &rangeRect)
{
    chart->axisX()->setRange(rangeRect.x(),rangeRect.width());
    chart->axisY()->setRange(rangeRect.y(),rangeRect.height());
}
