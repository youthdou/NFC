#ifndef NFCFIRMWAREVIEW_H
#define NFCFIRMWAREVIEW_H

#include <QFrame>

namespace Ui {
class NFCFirmwareView;
}

class NFCFirmwareView : public QFrame
{
    Q_OBJECT

public:
    explicit NFCFirmwareView(QWidget *parent = 0);
    ~NFCFirmwareView();
    void show_firmware(quint8 ic, quint8 ver, quint8 rev, quint8 support);

private slots:
    void on_getFirmwareBtn_clicked();

signals:
    void get_firmware_cmd();

private:
    Ui::NFCFirmwareView *ui;
};

#endif // NFCFIRMWAREVIEW_H
