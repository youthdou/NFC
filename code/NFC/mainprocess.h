#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>

#include "pn532/pn532.h"

#include "general_view/serialconfigview.h"

#include "view/nfcfirmwareview.h"
#include "view/nfctargetlistview.h"

namespace Ui {
class MainProcess;
}

class MainProcess : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainProcess(QWidget *parent = 0);
    ~MainProcess();

private slots:
    void on_portConfigAction_triggered();
    void on_openNFCAction_triggered();
    void process_nfc_cmd_get_firmware();
    void process_nfc_cmd_list_target();

private:
    Ui::MainProcess *ui;

    SerialConfigView *serial_config;
    NFCFirmwareView *nfc_firmware_view;
    NFCTargetListView *nfc_target_list_view;

    PN532 *nfc_module;
    bool nfc_module_on;

private:
    void initViews();

};

#endif // MAINPROCESS_H
