#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <vector>
#include <time.h>

#include <QtCharts>
#include <QTimer>
#include <QtCharts/QValueAxis>

//#define DYNAMIC_AXIS_SCALE

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

        void on_timerFinished();
    private:
        void getAvalilableSerialDevices();
        void serialRead();
        void serialWrite(QString message);

        void addChartPoint(const QPointF &point);
        void setChartRange(const QRectF &rangeRect);

        Ui::MainWindow *ui;

        std::string endOfMessage;
        qint32 baudrate;
        QSerialPort *usbDevice;
        std::vector<QSerialPortInfo> serialComPortList; //A list of the available ports for the dropdownmenue in the GUI

        QString deviceDescription;
        std::string serialBuffer;

        bool serialDeviceIsConnected;

        QLineSeries *dataSeries;

        QChart *chart;
        QChartView *chartView;
        int maxChartPoints;

        QRectF chartAxisRange;

        QTimer *timer;

        std::chrono::high_resolution_clock::time_point t1;
        std::chrono::high_resolution_clock::time_point t2;
        std::chrono::duration<double> time_span;
        double averageTimeSpan;
        unsigned long messageCounter;

};

#endif // MAINWINDOW_H
