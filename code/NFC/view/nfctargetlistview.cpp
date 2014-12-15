#include "nfctargetlistview.h"
#include "ui_nfctargetlistview.h"

NFCTargetListView::NFCTargetListView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::NFCTargetListView)
{
    ui->setupUi(this);
}

NFCTargetListView::~NFCTargetListView()
{
    delete ui;
}

void NFCTargetListView::show_uid(QString uid)
{
    ui->uidLineEdit->setText(uid);
}

void NFCTargetListView::on_searchBtn_clicked()
{
    emit list_passive_target_cmd();
}
