#include "hardwarereader.h"
#include <QDebug>
#include <QDataStream>

hardwareReader::hardwareReader(QObject *parent)
    : QObject(parent), active(false)
{
}

hardwareReader::~hardwareReader()
{}

bool hardwareReader::setSerialPort(QString _port, int _baudrate)
{
    active = false;

    if(!this->m_serialPort.isNull()) {
        disconnect(m_serialPort.data(), &QSerialPort::readyRead, this, &hardwareReader::slotHandleReadyRead);
        disconnect(m_serialPort.data(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                   this, &hardwareReader::slotHandleError);
        if(this->m_serialPort->isOpen()){
            this->m_serialPort->close();
        }
    }

    QSharedPointer<QSerialPort> _tSerialPort(new QSerialPort);
    _tSerialPort->setPortName(_port);

    if(_tSerialPort->open(QIODevice::ReadOnly)) {
        _tSerialPort->setParity(QSerialPort::NoParity);
        _tSerialPort->setBaudRate(_baudrate);
        _tSerialPort->setStopBits(QSerialPort::OneStop);
        _tSerialPort->setDataBits(QSerialPort::Data8);

        this->m_serialPort = _tSerialPort;

        connect(m_serialPort.data(), &QSerialPort::readyRead, this, &hardwareReader::slotHandleReadyRead);
        connect(m_serialPort.data(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, &hardwareReader::slotHandleError);

        emit sigLog("Hardware", "Successfully connected");

        return true;
    } else {

        emit sigLog("Hardware", "Could not connect to the specified port");
        return false;
    }

}

bool hardwareReader::setSerialPort(QSharedPointer<QSerialPort> _serialPort)
{
    if(!this->m_serialPort.isNull()) {
        disconnect(m_serialPort.data(), &QSerialPort::readyRead, this, &hardwareReader::slotHandleReadyRead);
        disconnect(m_serialPort.data(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                   this, &hardwareReader::slotHandleError);
    }
    if(_serialPort->isOpen()) {

        this->m_serialPort = _serialPort;

        connect(m_serialPort.data(), &QSerialPort::readyRead, this, &hardwareReader::slotHandleReadyRead);
        connect(m_serialPort.data(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, &hardwareReader::slotHandleError);

        emit sigLog("hardware", "Successfully connected");

        return true;
    } else {
        emit sigLog("hardware", "Not active");
        return false;
    }
}

void hardwareReader::slotHandleReadyRead()
{
    m_readData.append(m_serialPort->readAll());
    QDataStream ds(m_readData);

    quint16 val;
    ds >> val;
    if(val < 15000 && !active){
        active = true;
        emit sigAlert();
    } else if(val > 15000) {
        active = false;
    }

    m_readData.clear();
}


void hardwareReader::slotHandleError(QSerialPort::SerialPortError _error)
{
    if (_error == QSerialPort::ReadError) {
        QString err = QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").
                arg(m_serialPort->portName()).arg(m_serialPort->errorString());

        emit sigLog("hardware", err);
    }
}

void hardwareReader::slotDisconnect()
{
    if(!this->m_serialPort.isNull() && this->m_serialPort->isOpen()) {
        disconnect(m_serialPort.data(), &QSerialPort::readyRead, this, &hardwareReader::slotHandleReadyRead);
        disconnect(m_serialPort.data(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                   this, &hardwareReader::slotHandleError);

        this->m_serialPort->close();

        emit sigLog("hardware", "Successfully disconnected");
    } else {
        emit sigLog("hardware", "No connection to close");

    }
}
