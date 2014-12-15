#include "mainprocess.h"
#include "ui_mainprocess.h"

MainProcess::MainProcess(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainProcess)
{
    ui->setupUi(this);
    nfc_module = new PN532(this);
    serial_config = new SerialConfigView(this);
    connect(ui->cmdListView, SIGNAL(currentRowChanged(int)),
            ui->cmdDetailView, SLOT(setCurrentIndex(int)));
    nfc_module_on = false;
    initViews();
}

MainProcess::~MainProcess()
{
    delete ui;
}

void MainProcess::on_portConfigAction_triggered()
{
    serial_config->exec();
}

void MainProcess::on_openNFCAction_triggered()
{
    if(nfc_module_on)
    {
        nfc_module_on = false;
        ui->openNFCAction->setText("Open NFC");
        nfc_module->pn532_close();
    }
    else
    {
        QString port_name;
        quint32 baudrate;
        if(!serial_config->getSerialConfig(port_name, baudrate))
            return;
        if(!nfc_module->pn532_open(port_name))
            return;
        ui->openNFCAction->setText("Close NFC");
        nfc_module_on = true;
    }
}

void MainProcess::process_nfc_cmd_get_firmware()
{
    if(!nfc_module_on)
    {
        QMessageBox::warning(this, "NFC Warning", "Open nfc first!");
        return;
    }
    pn532_version_t version;
    if(!nfc_module->pn532_get_firmware_version(version))
        return;
    nfc_firmware_view->show_firmware(version.ic, version.ver,
                                     version.rev, version.support);
}

void MainProcess::process_nfc_cmd_list_target()
{
    if(!nfc_module_on)
    {
        QMessageBox::warning(this, "NFC Warning", "Open nfc first!");
        return;
    }
    QByteArray uid;
    if(!nfc_module->pn532_list_passive_target(uid))
        return;
    nfc_target_list_view->show_uid(QString(uid.toHex().toUpper()));
}

void MainProcess::initViews()
{
    nfc_target_list_view = new NFCTargetListView();
    ui->cmdDetailView->addWidget(nfc_target_list_view);
    connect(nfc_target_list_view, SIGNAL(list_passive_target_cmd()),
            this, SLOT(process_nfc_cmd_list_target()));

    nfc_firmware_view = new NFCFirmwareView();
    ui->cmdDetailView->addWidget(nfc_firmware_view);
    connect(nfc_firmware_view, SIGNAL(get_firmware_cmd()),
            this, SLOT(process_nfc_cmd_get_firmware()));

    ui->cmdDetailView->setCurrentIndex(0);

}
