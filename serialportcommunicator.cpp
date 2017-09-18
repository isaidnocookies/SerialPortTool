#include "serialportcommunicator.h"

SerialPortCommunicator::SerialPortCommunicator(QSerialPort *serialPort, QObject *parent)
    : QObject(parent)
    , m_serialPort(serialPort)
{
    m_timer.setSingleShot(true);

    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPortCommunicator::handleReadyRead);
    connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialPortCommunicator::handleError);

    connect(m_serialPort, &QSerialPort::bytesWritten, this, &SerialPortCommunicator::handleBytesWritten);
    connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialPortCommunicator::handleError);
    connect(&m_timer, &QTimer::timeout, this, &SerialPortCommunicator::handleTimeout);
}

SerialPortCommunicator::~SerialPortCommunicator()
{
}



void SerialPortCommunicator::handleReadyRead()
{
    m_readData.append(m_serialPort->readAll());
    emit sendMessage(m_readData);
}

void SerialPortCommunicator::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug() << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }

    if (serialPortError == QSerialPort::WriteError) {
        qDebug() << QObject::tr("An I/O error occurred while writing the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }
}

void SerialPortCommunicator::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.size()) {
        m_bytesWritten = 0;
        qDebug() << QObject::tr("Data successfully sent to port %1").arg(m_serialPort->portName()) << endl;
    }
}

void SerialPortCommunicator::handleTimeout()
{
    qDebug() << QObject::tr("Operation timed out for port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
}

void SerialPortCommunicator::write(const QByteArray &writeData)
{
    m_writeData = writeData;

    qint64 bytesWritten = m_serialPort->write(writeData);

    if (bytesWritten == -1) {
        qDebug() << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        return;
    } else if (bytesWritten != m_writeData.size()) {
        qDebug() << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        return;
    }

    m_timer.start(5000);
}

void SerialPortCommunicator::sendMessageToPort(QByteArray iMessage)
{
    const QByteArray lMessage = iMessage;
    write(lMessage);
}
