#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    usbDevice = new QSerialPort(this);
    connect(usbDevice,SIGNAL(readyRead()),this,SLOT(onSerialDataAvailable()));

    baudrate = QSerialPort::Baud115200;

    serialDeviceIsConnected = false;
    getAvalilableSerialDevices();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete usbDevice;
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
        serialBuffer += usbDevice->readAll(); // Read the available data
    }
}
void MainWindow::onSerialDataAvailable()
{
    if(serialDeviceIsConnected == true)
    {
        serialRead(); // Read a chunk of the Message
        //To solve that problem I send a end char "]" in My case. That helped my to know when a message is complete

        if(serialBuffer.indexOf("]") != -1) //Message complete
        {
            qDebug() << "Message from device: "<<serialBuffer;
            serialWrite("echoFromGui");

            //Do something with de message here

            serialBuffer = "";  //Clear the buffer;
        }
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
        }
        else
        {
            qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " not connected";
            qDebug() <<"         Error: "<<usbDevice->errorString();
            serialDeviceIsConnected = false;
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
        qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " closed";
    }
    else
    {
       qDebug() << "Can't disconnect, no device is connected";
    }
}
