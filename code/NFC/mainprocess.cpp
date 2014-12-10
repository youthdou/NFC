#include "mainprocess.h"
#include "ui_mainprocess.h"

MainProcess::MainProcess(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainProcess)
{
    ui->setupUi(this);
    nfc_module = new PN532(this);
    nfc_module->pn532_open("COM1");
}

MainProcess::~MainProcess()
{
    delete ui;
}
