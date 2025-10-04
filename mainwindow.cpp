#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QDataStream>

constexpr int BAUDRATE = 460800;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
    , responseTimer(new QTimer(this))
{
    ui->setupUi(this);

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBox->addItem(info.portName());

    connect(ui->comboBox,SIGNAL(activated(const QString &)),
            this, SLOT(onPortChanged(const QString &)));

    connect(serial,&QSerialPort::readyRead, this, &MainWindow::readData);

    responseTimer->setSingleShot(true);
    connect(responseTimer, &QTimer::timeout, this, &MainWindow::onResponseTimeout);
    continuousTimer = new QTimer(this);

    int frequency=1237;
    QByteArray freqBytes;
    QDataStream stream(&freqBytes,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream<<frequency;
    qDebug()<<freqBytes;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPortChanged(const QString &portName)
{
    if (serial->isOpen()) {
        serial->close();
    }

    serial->setPortName(portName);
    serial->setBaudRate(BAUDRATE);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        QString msg = QString("Serial port %1 opened successfully").arg(portName);
        QMessageBox::information(this,"Success",msg);
    } else {
        QMessageBox::warning(this,"Error","Serial Port is not open");
    }
}

void MainWindow::readData()
{
    serialBuffer.append(serial->readAll());
    responseTimer->start(50);  // restart 50 ms timeout after last data
}

void MainWindow::onResponseTimeout()
{
    qDebug()<<serialBuffer.toHex();
    QVector<quint16> highg_buffer;
    QVector<quint16> highg_buffer1;
    QByteArray hexbytes;
    QByteArray hexbytes2;

    QVector<quint16> highg_buffer3;
    QByteArray hexbytes3;

    for(int i=0;i<510;i++)
    {
        highg_buffer.append(i);

    }

    for(auto num:highg_buffer)
    {
        hexbytes.append(static_cast<quint8>(num>>8)&0xff);
        hexbytes.append(static_cast<quint8>(num&0xff));
    }

    for(int i=510;i>0;i--)
    {
        highg_buffer1.append(i);

    }

    for(auto num:highg_buffer1)
    {
        hexbytes2.append(static_cast<quint8>(num>>8)&0xff);
        hexbytes2.append(static_cast<quint8>(num&0xff));
    }

    for(int i=510;i<1020;i++)
    {
        highg_buffer3.append(i);

    }

    for(auto num:highg_buffer3)
    {
        hexbytes3.append(static_cast<quint8>(num>>8)&0xff);
        hexbytes3.append(static_cast<quint8>(num&0xff));
    }

    //qDebug()<<hexbytes;
    //quint16 chk_val=(static_cast<quint8>(hexbytes[1018]))<<8|(static_cast<quint8>(hexbytes[1019]));

    //qDebug()<<chk_val;
    if(serialBuffer.size() > 0)
    {
        qDebug() << "received";
        QByteArray data=QByteArray::fromHex("FF03FF");
        QByteArray data2=QByteArray::fromHex("FF0DFF");

        if(serialBuffer==data)
        {
            series=0;
            hexbytes.prepend(QByteArray::fromHex("AABBCCDDEEFF"));
            qDebug()<<hexbytes.size();
            serial->write(hexbytes);
            qDebug()<<hexbytes.toHex();
            count++;
            qDebug()<<count;
            ui->plainTextEdit->appendPlainText("ReceivedData : "+serialBuffer.toHex());
        }

        if(serialBuffer==data2)
        {
            ui->plainTextEdit->appendPlainText("ReceivedData : "+serialBuffer.toHex());
            series++;
            count++;
            if(series<6144) //38000 is maxed out, 9000 is for type I almost give 45 lakh points, 6144 actual hardware value.
            {
                qDebug()<<"send continuous data";

                if(count%2==0)
                {
                    hexbytes2.prepend(QByteArray::fromHex("AABBCCDDEEFF"));
                    QTimer::singleShot(1, this, [=]() { serial->write(hexbytes2); });
                    //qDebug()<<hexbytes2.toHex();

                }
                else if(series > 500 && series < 1000)
                {
                    hexbytes3.prepend(QByteArray::fromHex("AABBCCDDEEFF"));
                    QTimer::singleShot(1, this, [=]() { serial->write(hexbytes3); });
                }
                else
                {
                    hexbytes.prepend(QByteArray::fromHex("AABBCCDDEEFF"));
                    QTimer::singleShot(1, this, [=]() { serial->write(hexbytes); });
                    //qDebug()<<"hexbytes1";
                }
                ui->plainTextEdit->appendPlainText("Sending Packets : "+QString::number(series));
            }
            else
            {
                QByteArray command=QByteArray::fromHex("FF09AABBCCFF");

                serial->write(command);
                qDebug()<<command;
            }
        }
    }
    serialBuffer.clear();
}



