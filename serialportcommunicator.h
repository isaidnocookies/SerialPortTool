#ifndef SERIALPORTCOMMUNICATOR_H
#define SERIALPORTCOMMUNICATOR_H

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QDebug>

QT_USE_NAMESPACE

class SerialPortCommunicator : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortCommunicator(QSerialPort *serialPort, QObject *parent = nullptr);
    ~SerialPortCommunicator();
    void write(const QByteArray &writeData);

public slots:
    void sendMessageToPort(QByteArray iMessage);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void handleTimeout();

signals:
    void sendMessage(QByteArray);

private:
    QSerialPort     *m_serialPort;
    QByteArray      m_readData;
    QByteArray      m_writeData;
    qint64          m_bytesWritten;
    QTimer          m_timer;
};

#endif // SERIALPORTCOMMUNICATOR_H
