#include "serialport.h"
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

serialport::serialport(QObject *parent) : QObject(parent) {
    serial = new QSerialPort(this);
    responseTimer=new QTimer(this);
connect(serial, &QSerialPort::readyRead, this, &serialport::readData);
 connect(responseTimer, &QTimer::timeout, this, &serialport::onResponseTimeout);

}

serialport::~serialport()
{
    delete serial;
}

QStringList serialport::availablePorts()
{
    QStringList ports;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ports<<info.portName();
    }
    return ports;
}

void serialport::setPORTNAME(const QString &portName)
{
    buffer.clear();

    if(serial->isOpen())
    {
        serial->close();
    }

    serial->setPortName(portName);
    serial->setBaudRate(921600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);


    if(!serial->open(QIODevice::ReadWrite))
    {
      emit portOpening("Failed to open port "+serial->portName());
    }
    else
    {
      emit portOpening("Serial port "+serial->portName()+" opened successfully at baud rate 9600");
    }
}
void serialport::writeData(const QByteArray &data) {
    if (serial->isOpen()) {
        serial->write(data);
        serial->flush();

    }
    else{
        emit portOpening("Failed to open port "+serial->portName());
    }
}
void serialport::readData()
{
    buffer.append(serial->readAll());
    responseTimer->start(1000);
//    if(buffer.size()>=4){
//        QByteArray cmd=buffer.left(4);
//        buffer.remove(0,4);
//        emit dataReceived(cmd);


    }
     // Signal to update UI
void serialport::onResponseTimeout()
{
    if (!buffer.isEmpty()) {
        emit dataReceived(buffer);
        buffer.clear();
    } else {
        emit portOpening("No response from hardware");
    }
    responseTimer->stop();
}
