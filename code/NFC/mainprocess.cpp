#include "mainprocess.h"
#include "ui_mainprocess.h"

MainProcess::MainProcess(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainProcess)
{
    ui->setupUi(this);
    nfc_module = new pn532(this);
    nfc_module->pn532_wake_up();
}

MainProcess::~MainProcess()
{
    delete ui;
}
