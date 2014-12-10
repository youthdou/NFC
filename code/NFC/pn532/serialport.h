#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = 0);
    bool open(const QString &name, const quint32 baudrate);
    bool isOpen();
    qint16 read(QByteArray &data);
    qint16 write(quint8 *data, quint16 len);
    void close();

private:
    QSerialPort *sp_fd;
    bool isOpen_flg;


signals:


};

#endif // SERIALPORT_H
