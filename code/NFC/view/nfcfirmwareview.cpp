#include "nfcfirmwareview.h"
#include "ui_nfcfirmwareview.h"

NFCFirmwareView::NFCFirmwareView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::NFCFirmwareView)
{
    ui->setupUi(this);
    ui->ISO14443A_CheckBox->setEnabled(false);
    ui->ISO14443B_CheckBox->setEnabled(false);
    ui->ISO18092_CheckBox->setEnabled(false);
}

NFCFirmwareView::~NFCFirmwareView()
{
    delete ui;
}

void NFCFirmwareView::show_firmware(quint8 ic, quint8 ver, quint8 rev, quint8 support)
{
    if(ic == 0x32)
        ui->icLineEdit->setText("PN532");
    ui->verLineEdit->setText(QString("%1").arg(ver));
    ui->revLineEdit->setText(QString("%1").arg(rev));
    if(support & 0x01)
	{
        ui->ISO14443A_CheckBox->setChecked(true);
	}
	else
	{
		ui->ISO14443A_CheckBox->setChecked(false);
	}
	if(support & 0x02)
	{
		ui->ISO14443B_CheckBox->setChecked(true);
	}
	else
	{
		ui->ISO14443B_CheckBox->setChecked(false);
	}
	if(support & 0x04)
	{
        ui->ISO18092_CheckBox->setChecked(true);
	}
	else
	{
		ui->ISO18092_CheckBox->setChecked(false);
	}
}

void NFCFirmwareView::on_getFirmwareBtn_clicked()
{
    emit get_firmware_cmd();
}
