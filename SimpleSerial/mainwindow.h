#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <vector>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();


    private slots:
        void onSerialDataAvailable();
        void on_connect_button_clicked();
        void on_disconnect_button_clicked();

    private:
        void getAvalilableSerialDevices();
        void serialRead();
        void serialWrite(QString message);

        Ui::MainWindow *ui;

        QSerialPort *usbDevice;
        std::vector<QSerialPortInfo> serialComPortList; //A list of the available ports for the dropdownmenue in the GUI

        QString deviceDescription;
        QString serialBuffer;

        bool serialDeviceIsConnected;
};

#endif // MAINWINDOW_H
