#include "serialport.h"
#include <QMessageBox>
#include <QDebug>

SerialPort::SerialPort(QObject *parent) :
    QObject(parent)
{
    sp_fd = new QSerialPort();
    isOpen_flg = false;
}

bool SerialPort::open(const QString &name, const quint32 baudrate)
{
    if(!sp_fd)
        sp_fd = new QSerialPort();
    qDebug() << "Open" << name;
    sp_fd->setPortName(name);
    if(sp_fd->open(QIODevice::ReadWrite) == false)
    {
        isOpen_flg = false;
        qDebug() << "Open serial error: " << sp_fd->errorString()
                 << ", " << sp_fd->error();
        return false;
    }
    sp_fd->setBaudRate(baudrate);
    sp_fd->setDataBits(QSerialPort::Data8);
    sp_fd->setParity(QSerialPort::NoParity);
    sp_fd->setStopBits(QSerialPort::OneStop);
    sp_fd->setFlowControl(QSerialPort::NoFlowControl);
    isOpen_flg = true;
    return true;
}

bool SerialPort::isOpen()
{
    return isOpen_flg;
}

qint16 SerialPort::read(QByteArray &data)
{
    if(!sp_fd)
        return -1;

    if(!sp_fd->isOpen())
        return -1;

    data = sp_fd->readAll();
    return data.size();
}

qint16 SerialPort::write(quint8 *data, quint16 len)
{
    qint16 l;
    if(!sp_fd)
        return -1;

    if(!sp_fd->isOpen())
        return -1;

    l = sp_fd->write((const char *)data, len);
    return l;
}

void SerialPort::close()
{
    if(sp_fd && sp_fd->isOpen())
    {
        qDebug("close serialport");
        sp_fd->close();
    }
    isOpen_flg = false;
}

