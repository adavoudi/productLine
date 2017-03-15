#ifndef HARDWAREREADER_H
#define HARDWAREREADER_H

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QByteArray>
#include <QObject>
#include <QSharedPointer>
#include <QTimer>


class hardwareReader : public QObject
{
    Q_OBJECT

public:
    explicit hardwareReader(QObject *parent = nullptr);
    ~hardwareReader();

    bool setSerialPort(QString _port, int _baudrate);
    bool setSerialPort(QSharedPointer<QSerialPort> _serialPort);

signals:
    void sigAlert();
    void sigLog(QString _title, QString _desc);

public slots:
    void slotDisconnect();

private slots:
    void slotHandleReadyRead();
    void slotHandleError(QSerialPort::SerialPortError _error);

private:
    QSharedPointer<QSerialPort> m_serialPort;
    QByteArray  m_readData;

    bool        active;
};

#endif // HARDWAREREADER_H
