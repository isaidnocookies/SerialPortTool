#ifndef SERIALPORTCOMMUNICATOR_H
#define SERIALPORTCOMMUNICATOR_H

#include <QObject>

class SerialPortCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortCommunicator(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SERIALPORTCOMMUNICATOR_H