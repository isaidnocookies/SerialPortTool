#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QMessageBox>

#include "serialportcommunicator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void connectToPort();

public slots:
    void getMessageFromPort(QByteArray iMessage);

signals:
    void sendMessageToPort(QByteArray oMessage);

protected:
    void displayPorts();

private slots:
    void on_refreshButton_released();
    void on_clearMessagesPushButton_released();
    void on_sendMessageButton_released();
    void on_portListWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_connectPortsButton_released();
    void on_closePortsButton_released();

private:
    Ui::MainWindow          *ui;
    SerialPortCommunicator  *mSerialPortComm        = Q_NULLPTR;
    QSerialPort             *mSerialPort            = Q_NULLPTR;
    QString                 mSelectedPortName;
};

#endif // MAINWINDOW_H
