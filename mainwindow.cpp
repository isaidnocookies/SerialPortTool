#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    displayPorts();

    ui->connectedPortMessagesGroupBox->setEnabled(false);
    ui->connectPortsButton->setEnabled(false);
    ui->closePortsButton->setEnabled(false);
    ui->progressBar->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getMessageFromPort(QByteArray iMessage)
{
    ui->incomingMessageTextBrowser->append(iMessage);
    ui->incomingMessageTextBrowser->append("<br>");
}

void MainWindow::displayPorts()
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {

        QListWidgetItem *newUsbPort = new QListWidgetItem();
        newUsbPort->setText(QString("%1  %2  %3  %4").arg(info.portName(),
                                                         QString::number(info.vendorIdentifier()),
                                                         QString::number(info.productIdentifier()),
                                                         info.manufacturer()));
        newUsbPort->setData(Qt::UserRole, info.portName());

        ui->portListWidget->addItem(newUsbPort);
    }
}

void MainWindow::on_refreshButton_released()
{
    ui->portListWidget->clear();
    displayPorts();
}

void MainWindow::on_clearMessagesPushButton_released()
{
    ui->incomingMessageTextBrowser->clear();
}

void MainWindow::on_sendMessageButton_released()
{
    QMessageBox msgBox;

    if (!mSerialPortComm) {
        msgBox.setText("No port connections. Please connect to a port.");
        return;
    } else {
        QTextDocument *doc = ui->sendMessageTextBrowser->document();
        QByteArray oMessage = doc->toPlainText().toUtf8();
        emit sendMessageToPort(oMessage);
        msgBox.setText("Message Sent");
    }

    msgBox.exec();
}

void MainWindow::on_portListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    bool foundPort = false;
    QString s = "";
    mSelectedPortName = item->data(Qt::UserRole).toString();

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if (info.portName() == mSelectedPortName) {
            s = QObject::tr("Port: ") + info.portName() + "\n"
                    + QObject::tr("Location: ") + info.systemLocation() + "\n"
                    + QObject::tr("Description: ") + info.description() + "\n"
                    + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                    + QObject::tr("Serial number: ") + info.serialNumber() + "\n"
                    + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";
            foundPort = true;
            break;
        }
    }

    if (foundPort) {
        ui->connectPortsButton->setEnabled(true);
        ui->portDetailsLabel->setText(s);
    }
}

void MainWindow::connectToPort()
{
    QMessageBox msgBox;
    bool lSuccess;

    if (mSerialPort) {
        mSerialPort->deleteLater();
        mSerialPort = Q_NULLPTR;
    }
    if (mSerialPortComm) {
        mSerialPortComm->deleteLater();
        mSerialPortComm = Q_NULLPTR;
    }

    mSerialPort = new QSerialPort(this);
    mSerialPort->setBaudRate(QSerialPort::Baud9600);
    mSerialPort->setPortName(mSelectedPortName);

    if (!mSerialPort->open(QIODevice::ReadWrite)) {
        msgBox.setText("Connection Failed!");
        lSuccess = false;
    } else {
        lSuccess = true;
    }

    mSerialPortComm = new SerialPortCommunicator(mSerialPort);

    if (lSuccess) {
        ui->closePortsButton->setEnabled(true);
        ui->connectedPortMessagesGroupBox->setEnabled(true);
        msgBox.setText("Connection Succesful!");

        connect(this, &MainWindow::sendMessageToPort, mSerialPortComm, &SerialPortCommunicator::sendMessageToPort);

    }
    ui->progressBar->setVisible(false);
    msgBox.exec();
}

void MainWindow::on_connectPortsButton_released()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Connect to Serial Port?", "Are you sure you want to connect to this serial port?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        connectToPort();
    }
}

void MainWindow::on_closePortsButton_released()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Disconnect from serial port?", "Are you sure you want to close all connections/ports?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (mSerialPortComm) {
            mSerialPortComm->deleteLater();
            mSerialPortComm = Q_NULLPTR;
        }
        if (mSerialPort) {
            mSerialPort->deleteLater();
            mSerialPort = Q_NULLPTR;
        }

        ui->connectPortsButton->setEnabled(false);
        ui->portDetailsLabel->clear();
        ui->connectedPortMessagesGroupBox->setEnabled(false);
    }
}
