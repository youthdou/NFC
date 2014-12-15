#include "serialconfigview.h"
#include "ui_serialconfigview.h"

#define     DEBUG       1

#if DEBUG
#include <QDebug>
#define     TRACE(...)      qDebug(__VA_ARGS__)
#else
#define     TRACE(...)
#endif

SerialConfigView::SerialConfigView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialConfigView)
{
    ui->setupUi(this);
    serialPortInit();
    setWindowTitle(QString::fromLocal8Bit("¥Æø⁄≈‰÷√"));
    configed_flg = false;
}

SerialConfigView::~SerialConfigView()
{
    delete ui;
}

bool SerialConfigView::isConfig()
{
    return configed_flg;
}

bool SerialConfigView::getSerialConfig(QString &name, quint32 &baudrate)
{
    name = serialName;
    baudrate = serialBaudRate;
    return configed_flg;
}

void SerialConfigView::serialPortInit()
{
    QList<QSerialPortInfo> seriallist = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo serial, seriallist)
    {
        int count = ui->portComboBox->count();
        if(count > 0)
        {
            QString tmp, now;
            int i;
            for(i = 0; i < count; i++)
            {
                now = ui->portComboBox->itemText(i).remove(0, 3);
                tmp = serial.portName().remove(0, 3);

                if(now.toInt() > tmp.toInt())
                {
                    ui->portComboBox->insertItem(i, serial.portName());
                    break;
                }
            }
            if(i == count)
                ui->portComboBox->addItem(serial.portName());

        }
        else
        {
            ui->portComboBox->addItem(serial.portName());
        }
        TRACE("%s", qPrintable(serial.portName()));
    }
    ui->portComboBox->setCurrentIndex(0);
}

void SerialConfigView::on_confirmBtn_clicked()
{
    serialName = ui->portComboBox->currentText();
    serialBaudRate = ui->baudRateComboBox->currentText().toInt();
    configed_flg = true;
    QDialog::accept();
}

void SerialConfigView::on_cancelBtn_clicked()
{
    configed_flg = false;
    QDialog::reject();
}
