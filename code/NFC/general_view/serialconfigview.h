#ifndef SERIALCONFIGVIEW_H
#define SERIALCONFIGVIEW_H

#include <QWidget>

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class SerialConfigView;
}

class SerialConfigView : public QDialog
{
    Q_OBJECT

public:
    explicit SerialConfigView(QWidget *parent = 0);
    ~SerialConfigView();
    bool getSerialConfig(QString &name, quint32 &baudrate);
    bool isConfig();

private slots:
    void on_confirmBtn_clicked();
    void on_cancelBtn_clicked();

private:
    void serialPortInit();

private:
    Ui::SerialConfigView *ui;
    bool configed_flg;
    QString serialName;
    quint32 serialBaudRate;
};

#endif // SERIALCONFIGVIEW_H
